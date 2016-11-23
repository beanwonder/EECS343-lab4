#ifndef _FILE_H_
#define _FILE_H_

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
};


// in-core file system types

struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  int flags;          // I_BUSY, I_VALID

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
                      // now we will map addrs[last] as tag block
  uint addrs[NDIRECT+2];
};

// both in data block and in memory form
struct Tag {
 char key[10];    // at most 10 bytes for key, including NULL
 char value[18];  // at most 18 bytes for value, including NULL
 int  used;        // 4 bytes available for bookkeeping, etc, if needed
                  // 1 for used 0 for not used
};

#define I_BUSY 0x1
#define I_VALID 0x2


// device implementations

struct devsw {
  int (*read)(struct inode*, char*, int);
  int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1

#endif // _FILE_H_
