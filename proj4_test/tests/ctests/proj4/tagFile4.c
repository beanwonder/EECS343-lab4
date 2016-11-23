/* call tagFile 16 times. Call tagFile again should fail. Call removeFileTag and then call tagFile should succeed. */
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
   char* key = "typd";
   char* val = "utility";
   int len = 7;

   int i;
   for (i = 0;i < 16;i++){
       key[3] += 1;
       int res = tagFile(fd, key, val, len);
       assert(res > 0);
   }

   key[3] += 1;
   int res = tagFile(fd, key, val, len);
   assert(res == -1);

   key = "type";
   res = removeFileTag(fd, key);
   assert(res == 1);

   res = tagFile(fd, key, val, len);
   assert(res > 0);

   char buf[7];
   int valueLength = getFileTag(fd, key, buf, 7);
   assert(valueLength == len);
   
   close(fd);

   for(i = 0; i < len; i++){
      char v_actual = buf[i];
      char v_expected = val[i];
      assert(v_actual == v_expected);
   }

   printf(1, "TEST PASSED\n");
   exit();
}
