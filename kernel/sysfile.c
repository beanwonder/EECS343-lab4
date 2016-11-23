#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"
#include "sysfunc.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=proc->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;

  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd] == 0){
      proc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_dup(void)
{
  struct file *f;
  int fd;
  
  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

int
sys_read(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return fileread(f, p, n);
}

int
sys_write(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return filewrite(f, p, n);
}

int
sys_close(void)
{
  int fd;
  struct file *f;
  
  if(argfd(0, &fd, &f) < 0)
    return -1;
  proc->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_fstat(void)
{
  struct file *f;
  struct stat *st;
  
  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
    return -1;
  return filestat(f, st);
}

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
  char name[DIRSIZ], *new, *old;
  struct inode *dp, *ip;

  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;
  if((ip = namei(old)) == 0)
    return -1;
  ilock(ip);
  if(ip->type == T_DIR){
    iunlockput(ip);
    return -1;
  }
  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);
  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  int off;
  struct dirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

int
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], *path;
  uint off;

  if(argstr(0, &path) < 0)
    return -1;
  if((dp = nameiparent(path, name)) == 0)
    return -1;
  ilock(dp);

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0){
    iunlockput(dp);
    return -1;
  }

  if((ip = dirlookup(dp, name, &off)) == 0){
    iunlockput(dp);
    return -1;
  }
  ilock(ip);

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    iunlockput(dp);
    return -1;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  if(ip->type == T_DIR){
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  return 0;
}

static struct inode*
create(char *path, short type, short major, short minor)
{
  uint off;
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);

  if((ip = dirlookup(dp, name, &off)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(dp->dev, type)) == 0)
    panic("create: ialloc");

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");

  iunlockput(dp);
  return ip;
}

int
sys_open(void)
{
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;
  if(omode & O_CREATE){
    if((ip = create(path, T_FILE, 0, 0)) == 0)
      return -1;
  } else {
    if((ip = namei(path)) == 0)
      return -1;
    ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip);
      return -1;
    }
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    iunlockput(ip);
    return -1;
  }
  iunlock(ip);

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}

int
sys_mkdir(void)
{
  char *path;
  struct inode *ip;

  if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0)
    return -1;
  iunlockput(ip);
  return 0;
}

int
sys_mknod(void)
{
  struct inode *ip;
  char *path;
  int len;
  int major, minor;
  
  if((len=argstr(0, &path)) < 0 ||
     argint(1, &major) < 0 ||
     argint(2, &minor) < 0 ||
     (ip = create(path, T_DEV, major, minor)) == 0)
    return -1;
  iunlockput(ip);
  return 0;
}

int
sys_chdir(void)
{
  char *path;
  struct inode *ip;

  if(argstr(0, &path) < 0 || (ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    return -1;
  }
  iunlock(ip);
  iput(proc->cwd);
  proc->cwd = ip;
  return 0;
}

int
sys_exec(void)
{
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(proc, uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(proc, uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      proc->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}

// tagfile
int sys_tagFile(void)
{
  // basically write the tags into the first data block 
  // now the first data block is no longer for file
  // should have changes in file.c 
  int fd;
  struct file *f;
  char *key;
  char *value;
  int key_len;
  int value_len;
  // struct Tag tag;
  struct Tag tags[BSIZE/sizeof(struct Tag)];

  if (argfd(0, &fd, &f) < 0)
    return -1;
  // need to be readable writable 
  if (f->readable == 0) 
    return -1;
  if(f->writable == 0)
    return -1;
  if (f->type == FD_PIPE)
    return -1;

  // not including null terminater key len
  key_len = argstr(1, &key);
  if (key_len >= 10 || key_len <= 0)
    return -1;
  if (argint(3, &value_len) < 0)
    return -1;
  if (value_len >= 18 || value_len < 0)
    return -1;
  if (argptr(2, &value, value_len) < 0)
    return -1;

  // get the tag block
  if (gettag(f, (char*) tags) < 0)
    return -1;

  // fill up my tag
  // safestrcpy(tag.key, key, key_len);
  // safestrcpy(tag.value, value, value_len);

  int i;
  // cprintf("value len: %d\n", value_len);
  for (i = 0; i < BSIZE/sizeof(struct Tag); ++i) {
    if (tags[i].used != 1 || strncmp(key, tags[i].key, key_len) == 0) {
      safestrcpy(tags[i].key, key, key_len+1);
      // tags[i].key[key_len] = 0;
      safestrcpy(tags[i].value, value, value_len+1);
      // tags[i].value[value_len] = 0;
      // cprintf("value in tagfile: %s\n", tags[i].value);
      tags[i].used = 1;
      break;
    }
  }

  if (i == BSIZE/sizeof(struct Tag)) {
    // no slot find
    return -1;
  }
  // 
  return updatetag(f, (char*) tags);
}


int sys_getFileTag(void) 
{
  int fd;
  struct file *f;
  int key_len;
  char *key;
  // should fill in the buf this time
  char *buf;
  int buf_len;

  // my tags in memory
  struct Tag tags[BSIZE/sizeof(struct Tag)];

  if (argfd(0, &fd, &f) < 0)
    return -1;
  
  // PERM check readable file
  if (f->readable == 0) 
    return -1;
  if (f->type == FD_PIPE)
    return -1;

  // not including NULL terminater
  key_len = argstr(1, &key);
  if (key_len >= 10 || key_len <= 0)
    return -1;
  if (argint(3, &buf_len) < 0)
    return -1;
  // if (buf_len < 18)
    // return -1;
  if (argptr(2, &buf, buf_len) < 0)
    return -1;
  // read the whole tag block
  // cprintf("before search1\n");
  if (gettag(f, (char*) tags) < 0)
    return -1;
  // iterate it and try to find the value
  int i;
  int val_len;
  // cprintf("before search2\n");
  for (i = 0; i < BSIZE/sizeof(struct Tag); ++i) {
    if (tags[i].used == 1 && strncmp(key, tags[i].key, key_len) == 0) {
      val_len = strlen(tags[i].value);
      // cprintf("sys_getFiletag: val len: %d buffer len %d\n", val_len, buf_len);
      if (val_len <= buf_len) {
        strncpy(buf, tags[i].value, val_len);
        // cprintf("value in gettag: %s\n", tags[i].value);
      }
      return val_len;
    }
  }
  // not find fail
  return -1;
}


int sys_removeFileTag(void) 
{
  int fd;
  struct file *f;
  int key_len;
  char *key;
  struct Tag tags[BSIZE/sizeof(struct Tag)];

  if (argfd(0, &fd, &f) < 0)
    return -1;
  // need to be readable writable 
  if (f->readable == 0) 
    return -1;
  if(f->writable == 0)
    return -1;
  if (f->type == FD_PIPE)
    return -1;

  // not including NULL terminater
  key_len = argstr(1, &key);
  if (key_len >= 10 || key_len <= 0)
    return -1;

  if (gettag(f, (char*) tags) < 0)
    return -1;

  int i;
  int target;
  int last;
  // first find the key tag
  for (i = 0; i < BSIZE/sizeof(struct Tag); ++i) {
    if (tags[i].used == 1 && strncmp(key, tags[i].key, key_len) == 0) {
      target = i;
      break;
    }
  }

  // not find
  if (i == BSIZE/sizeof(struct Tag))
    return -1;

  // find the last one
  for (; i < BSIZE/sizeof(struct Tag); ++i) {
    if (tags[i].used == 1) {
      last = i;
    } else {
      // find the last one
      break;
    }
  }

  tags[target] = tags[last];
  tags[last].used = 0;
  // move the last one to the hole above
  // update the tag block
  return updatetag(f, (char*) tags);
}

int sys_getAllTags(void)
{
  int fd;
  struct file *f;
  char *keys;
  int maxTags;
  struct Tag tags[BSIZE/sizeof(struct Tag)];

  if (argfd(0, &fd, &f) < 0)
    return -1;
  // PERM check readable file
  if (f->readable == 0) 
    return -1;
  if (f->type == FD_PIPE)
    return -1;
  if (argint(2, &maxTags) < 0)
    return -1;
  if (argptr(1, &keys, maxTags * sizeof(struct Key)) < 0)
    return -1;

  // get my tags
  if (gettag(f, (char*) tags) < 0)
    return -1;

  int i = 0;
  int cnt = 0;
  int n;
  struct Key *kArr = (struct Key*) keys;

  for (i = 0; i < BSIZE/sizeof(struct Tag); ++i) {
    if (tags[i].used == 1) {
      if (cnt <= maxTags) {
        n = strlen(tags[i].key);
        safestrcpy(kArr[cnt].key, tags[i].key, n+1);
      }
      cnt += 1;
    } else {
      break;
    }
  }
  return cnt;
}

