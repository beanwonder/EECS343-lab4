
/* call tagFile twice to tag a file twice.  Call getFileTag to read the tag of that file. */
#include "types.h"
#include "user.h"

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200

#undef NULL
#define NULL ((void*)0)


volatile int global = 1;
int ppid;
#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

int
main(int argc, char *argv[])
{
   ppid = getpid();
   int fd = open("ls", O_RDWR);
   printf(1, "fd of ls: %d\n", fd);
   char buf1[7];
   char* key = "type"; 
   int valueLength = getFileTag(fd, key, buf1, 7);
   assert(valueLength == -1);
  
   char* val = "utility";
   int len = 7;
   int res = tagFile(fd, key, val, len);
   assert(res > 0);
   
   valueLength = getFileTag(fd, key, buf1, 7);
   assert(valueLength == 7);
   assert(strcmp(buf1, val));
    
   valueLength = getFileTag(fd, key, buf1, 5);
   assert(valueLength == 7);
   //assert(strcmp(buf1, val));
 
   key = "dummy"; 
   valueLength = getFileTag(fd, key, buf1, 7);
   assert(valueLength == -1);
  // assert(strcmp(buf1, val));
  
/*
   len = 10;
   val = "jokesonyou";
   res = tagFile(fd, key, val, len);
   assert(res > 0);

   char buf[10];
   int valueLength = getFileTag(fd, key, buf, 10);
   printf(1, "valueLength is %d\n", valueLength);
   assert(valueLength == len);

   close(fd);

   int i;
   for(i = 0; i < len; i++){
      char v_actual = buf[i];
      char v_expected = val[i];
      assert(v_actual == v_expected);
   }
*/
   printf(1, "TEST PASSED\n");
   exit();
}
