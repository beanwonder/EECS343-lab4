/* call tagFile with corner parameters.  Call getFileTag to read the tag of that file. */
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
   int fd_read = open("pwd", O_RDONLY);
   int fd = open("ls", O_RDWR);
   printf(1, "fd of ls: %d\n", fd);
   
   // key is 9 bytes
   char* key = "keylenis9";
   char* val = "utility";
   int len = 7;
   // also check if returns -1 when fd is readonly
   int res = tagFile(fd_read, key, val, len);
   assert(res == -1);
   printf(1, "key is %s 1\n", key);
   close(fd_read);
   res = tagFile(fd, key, val, len);
   printf(1, "key is %s 2 \n", key);
   assert(res > 0);

   char buf[7];
   int valueLength = getFileTag(fd, key, buf, 7);
   printf(1, "key 3 value is %d\n", valueLength);
   assert(valueLength == len);

   int i;
   for(i = 0; i < len; i++){
      char v_actual = buf[i];
      char v_expected = val[i];
      assert(v_actual == v_expected);
   }

   // key is 1 byte
   key = "l";
   res = tagFile(fd, key, val, len);
   assert(res > 0);

   valueLength = getFileTag(fd, key, buf, 7);
   assert(valueLength == len);

   for(i = 0; i < len; i++){
      char v_actual = buf[i];
      char v_expected = val[i];
      assert(v_actual == v_expected);
   }

   // key is 10 bytes
   key = "keylenis10";
   res = tagFile(fd, key, val, len);
   assert(res == -1);

   // key is 1 byte
   key = "";
   res = tagFile(fd, key, val, len);
   assert(res == -1);

   // value is 17 bytes
   key = "type";
   val = "valueisseventeen~";
   len = 17;
   res = tagFile(fd, key, val, len);
   assert(res > 0);

   char buf2[17];
   valueLength = getFileTag(fd, key, buf2, 17);
   assert(valueLength == len);

   for(i = 0; i < len; i++){
      char v_actual = buf2[i];
      char v_expected = val[i];
      assert(v_actual == v_expected);
   }

   close(fd);
   printf(1, "TEST PASSED\n");
   exit();
}
