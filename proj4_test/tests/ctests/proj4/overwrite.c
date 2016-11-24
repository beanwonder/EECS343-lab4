/* To check that if two tags are written with the same key, the first value gets overwritten. */
#include "types.h"
#include "user.h"

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200

#undef NULL
#define NULL ((void*)0)

int ppid;
volatile int global = 1;

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
   char* key = "imakey";
   char* val1 = "value1";
   char* val2 = "value2";
   int len1 = 6;
   int len2 = 6;
   int res1 = tagFile(fd, key, val1, len1);
   assert(res1 > 0);

   char buf1[6];
   int valueLength1 = getFileTag(fd, key, buf1, 6);
   assert(valueLength1 == len1);

   int res2 = tagFile(fd, key, val2, len2);
   assert(res2 > 0);

   char buf2[6];
   int valueLength2 = getFileTag(fd, key, buf2, 6);
   assert(valueLength2 == len2);

   int i;
   for(i = 0; i < len2; i++){
      char v_actual = buf2[i];
      char v_expected = val2[i];
      assert(v_actual == v_expected);
   }

   close(fd);
   printf(1, "TEST PASSED\n");
   exit();
}
