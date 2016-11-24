/* Trying to add 16 tags and remove them all. */
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
   char* key1 = "key1";
   char* val1 = "value1";
   char* key2 = "key2";
   char* val2 = "value2";
   char* key3 = "key3";
   char* val3 = "value3";
   char* key4 = "key4";
   char* val4 = "value4";
   char* key5 = "key5";
   char* val5 = "value5";
   char* key6 = "key6";
   char* val6 = "value6";
   char* key7 = "key7";
   char* val7 = "value7";
   char* key8 = "key8";
   char* val8 = "value8";
   char* key9 = "key9";
   char* val9 = "value9";
   char* key10 = "key10";
   char* val10 = "value10";
   char* key11 = "key11";
   char* val11 = "value11";
   char* key12 = "key12";
   char* val12 = "value12";
   char* key13 = "key13";
   char* val13 = "value13";
   char* key14 = "key14";
   char* val14 = "value14";
   char* key15 = "key15";
   char* val15 = "value15";
   char* key16 = "key16";
   char* val16 = "value16";
   
   int len1_9 = 6;
   int len10_16 = 7;
   
   int res1 = tagFile(fd, key1, val1, len1_9);
   assert(res1 > 0);
   int res2 = tagFile(fd, key2, val2, len1_9);
   assert(res2 > 0);
   int res3 = tagFile(fd, key3, val3, len1_9);
   assert(res3 > 0);
   int res4 = tagFile(fd, key4, val4, len1_9);
   assert(res4 > 0);
   int res5 = tagFile(fd, key5, val5, len1_9);
   assert(res5 > 0);
   int res6 = tagFile(fd, key6, val6, len1_9);
   assert(res6 > 0);
   int res7 = tagFile(fd, key7, val7, len1_9);
   assert(res7 > 0);
   int res8 = tagFile(fd, key8, val8, len1_9);
   assert(res8 > 0);
   int res9 = tagFile(fd, key9, val9, len1_9);
   assert(res9 > 0);
   int res10 = tagFile(fd, key10, val10, len10_16);
   assert(res10 > 0);
   int res11 = tagFile(fd, key11, val11, len10_16);
   assert(res11 > 0);
   int res12 = tagFile(fd, key12, val12, len10_16);
   assert(res12 > 0);
   int res13 = tagFile(fd, key13, val13, len10_16);
   assert(res13 > 0);
   int res14 = tagFile(fd, key14, val14, len10_16);
   assert(res14 > 0);
   int res15 = tagFile(fd, key15, val15, len10_16);
   assert(res15 > 0);
   int res16 = tagFile(fd, key16, val16, len10_16);
   assert(res16 > 0);

   
   char buf1_9[6];
   char buf10_16[7];

   int res1_2 = removeFileTag(fd, key1);
   assert(res1_2 > 0);
   int valueLength1 = getFileTag(fd, key1, buf1_9, 6);
   printf(1, "valuelenth1: %d\n", valueLength1);
   printf(1, "value1: %s\n", buf1_9);
   assert(valueLength1 == -1);

   int res2_2 = removeFileTag(fd, key2);
   assert(res2_2 > 0);
   int valueLength2 = getFileTag(fd, key2, buf1_9, 6);
   assert(valueLength2 == -1);

   int res3_2 = removeFileTag(fd, key3);
   assert(res3_2 > 0);
   int valueLength3 = getFileTag(fd, key3, buf1_9, 6);
   assert(valueLength3 == -1);

   int res4_2 = removeFileTag(fd, key4);
   assert(res4_2 > 0);
   int valueLength4 = getFileTag(fd, key4, buf1_9, 6);
   assert(valueLength4 == -1);

   int res5_2 = removeFileTag(fd, key5);
   assert(res5_2 > 0);
   int valueLength5 = getFileTag(fd, key5, buf1_9, 6);
   assert(valueLength5 == -1);

   int res6_2 = removeFileTag(fd, key6);
   assert(res6_2 > 0);
   int valueLength6 = getFileTag(fd, key6, buf1_9, 6);
   assert(valueLength6 == -1);

   int res7_2 = removeFileTag(fd, key7);
   assert(res7_2 > 0);
   int valueLength7 = getFileTag(fd, key7, buf1_9, 6);
   assert(valueLength7 == -1);

   int res8_2 = removeFileTag(fd, key8);
   assert(res8_2 > 0);
   int valueLength8 = getFileTag(fd, key8, buf1_9, 6);
   assert(valueLength8 == -1);

   int res9_2 = removeFileTag(fd, key9);
   assert(res9_2 > 0);
   int valueLength9 = getFileTag(fd, key9, buf1_9, 6);
   assert(valueLength9 == -1);

   int res10_2 = removeFileTag(fd, key10);
   assert(res10_2 > 0);
   int valueLength10 = getFileTag(fd, key10, buf10_16, 6);
   assert(valueLength10 == -1);

   int res11_2 = removeFileTag(fd, key11);
   assert(res11_2 > 0);
   int valueLength11 = getFileTag(fd, key11, buf10_16, 7);
   assert(valueLength11 == -1);

   int res12_2 = removeFileTag(fd, key12);
   assert(res12_2 > 0);
   int valueLength12 = getFileTag(fd, key12, buf10_16, 7);
   assert(valueLength12 == -1);

   int res13_2 = removeFileTag(fd, key13);
   assert(res13_2 > 0);
   int valueLength13 = getFileTag(fd, key13, buf10_16, 7);
   assert(valueLength13 == -1);

   int res14_2 = removeFileTag(fd, key14);
   assert(res14_2 > 0);
   int valueLength14 = getFileTag(fd, key14, buf10_16, 7);
   assert(valueLength14 == -1);

   int res15_2 = removeFileTag(fd, key15);
   assert(res15_2 > 0);
   int valueLength15 = getFileTag(fd, key15, buf10_16, 7);
   assert(valueLength15 == -1);

   int res16_2 = removeFileTag(fd, key16);
   assert(res16_2 > 0);
   int valueLength16 = getFileTag(fd, key16, buf10_16, 7);
   assert(valueLength16 == -1);


   close(fd);
   printf(1, "TEST PASSED\n");
   exit();
}

