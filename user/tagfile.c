#include "types.h"
#include "user.h"

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200

int
main(int argc, char *argv[])
{
 int fd = open("ls", O_RDWR);

 char* key = "type";
 char* val = "utility";
 int len = 7;
 int res = tagFile(fd, key, val, len); // tag file as "type": "utility"
 if(res < 0){
  printf(1, "tagFile error.\n");
 }
 // close(fd);
 // fd = open("ls", O_RDONLY);
 key = "type";
 char buffer[18];
 res = getFileTag(fd, key, buffer, 18);
 if (res <= 18) {
    // printf(1, "res: %d\n", res);
    printf(1, "%s: %s\n", key, buffer);  // prints "type: utility" (assuming tagFile 
                                        // was previously used to set the tag value as "utility"
 } else {
   printf(1, "buffer too small.\n");
 }

 val = "changed";
 len = 7;
 res = tagFile(fd, key, val, len); // tag file as "type": "utility"
 if (res < 0) {
	 printf(1, "tagfile error: second tag\n");
 }

 key = "name";
 val = "tom";
 len = 3;
 res = tagFile(fd, key, val, len);
 if (res < 0) {
 	printf(1, "tagfile error: new tag name");
 }

 key = "type";
 res = getFileTag(fd, key, buffer, 18);
 if(res <= 18){
    // printf(1, "res: %d\n", res);
    printf(1, "%s: %s\n", key, buffer);  // prints "type: utility" (assuming tagFile 
                               // was previously used to set the tag value as "utility"
 } else {
   printf(1, "buffer too small.\n");
 }

 key = "name";

 res = getFileTag(fd, key, buffer, 18);
 if (res <= 18) {
    printf(1, "%s: %s\n", key, buffer);  // prints "type: utility" (assuming tagFile 
 } else {
   printf(1, "buffer too small.\n");
 }


 if (removeFileTag(fd, "name") != 1) {
 	printf(1, "removeFileTage fails\n");
 }

 if (getFileTag(fd, "name", buffer, 18) >= 0) {
 	printf(1, "get removed tag name\n");
 }

 if (removeFileTag(fd, "type") != 1) {
 	printf(1, "removeFileTag fails\n");
 }
 
 if (getFileTag(fd, "type", buffer, 18) >= 0) {
 	printf(1, "get removed tag type\n");
 }


 // finally add a tag into file
 key = "type";
 val = "utility";
 len = 7;
 res = tagFile(fd, key, val, len); // tag file as "type": "utility"
 if(res < 0){
  printf(1, "tagFile error.\n");
 }

 close(fd);
 exit();
}