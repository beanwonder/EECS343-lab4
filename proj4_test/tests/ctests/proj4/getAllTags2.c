#include "types.h"
#include "user.h"
// make sure that struct Key is included via either types.h or user.h above

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200


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
    char * key[3];
    char * vals[3];
    int fd = open("ls", O_RDWR);
    int res;
    int len = 8;
    key[0] = "type1";
    vals[0] = "utility1";
    res = tagFile(fd, key[0], vals[0], len); 
    assert(res > 0);
    key[1] = "type2";
    vals[1] = "utility2";
    res = tagFile(fd, key[1], vals[1], len);  
    assert(res > 0);
    key[2] = "type3";
    vals[2] = "utility3";
    res = tagFile(fd, key[2], vals[2], len);  
    assert(res > 0);
    struct Key keys[3];
    int numTags = getAllTags(fd, keys, 3);
    assert(numTags == 3);
    int i, j;
    // const char * buffer_val;
    for(i = 0; i < numTags; i++){
        char buffer[8];
        char *expected_val = vals[i];
        int len = getFileTag(fd, keys[i].key, buffer, 8);
        assert(len == 8);
        for (j = 0; j < len; j++) {
            char v_actual = buffer[j];
            assert(v_actual == expected_val[j]);
        }
    }
    close(fd);
    printf(1, "getAllTags test passed\n");
    exit();
}
