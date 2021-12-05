#include "tinyweb.h"
#define MAX_LEN 1024


int test_rio_read_and_rio_write(char *filename)
{
    char *buf = "hello world";
    char  read_buf[MAX_LEN];
    int   fd  = -1;

    memset(read_buf, 0, MAX_LEN);

    if ((fd = open(filename, O_RDWR | O_CREAT, 0644)) < 0) {
        printf("Error: fail to open %s\n", filename);
        close(fd);
        return -1;
    }

    int ch_wrote = rio_writen(fd, buf, 11);

    if (ch_wrote < 0) {
        printf("Error: write fail\n");
        close(fd);
        return -1;
    }
    else if (ch_wrote == 0)
        printf("Success: write down\n");
    else
        printf("Success: write %d char\n", ch_wrote);

    lseek(fd, 0, SEEK_SET);
    int ch_read = rio_readn(fd, read_buf, 1024);

    if (ch_read < 0) {
        printf("Error: read fail\n");
        close(fd);
        return -1;
    }
    else if (ch_read == 0)
        printf("Success: read down\n");
    else
        printf("Success: read %d char\n", ch_read);
    
    printf("content: %s\n", read_buf);

    close(fd);
    return 0;
}


int test_rio_readn(char *filename)
{
    char  read_buf[MAX_LEN];
    int   fd  = -1;

    memset(read_buf, 0, MAX_LEN);

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: fail to open %s\n", filename);
        return -1;
    }

    int ch_read = rio_readn(fd, read_buf, 1024);

    if (ch_read < 0) {
        printf("Error: read fail\n");
        close(fd);
        return -1;
    }

    else if (ch_read == 0)
        printf("Success: read down\n");
    else
        printf("Success: read %d char\n", ch_read);
    
    printf("content: %s\n", read_buf);

    close(fd);
    return 0;

}


int test_rio_readnb(char *filename)
{
    rio_t  rt;
    rio_t *rp = &rt;
    int    fd = -1;
    int    n  = 3;
    char buf[MAX_LEN];

    memset(buf, 0, MAX_LEN);

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: fail to open %s\n", filename);
        return -1;
    }

    rio_readinitb(rp, fd);

    if (rio_readnb(rp, buf, n) >= 0) {
        printf("Readnb: %s\n", buf);
    }

    // change file content;
    scanf("%s", buf);
    printf("%s\n", buf);

    if (rio_readnb(rp, buf, 1024) >= 0) {
        printf("Readnb: %s\n", buf);
    }

    close(fd);
    return -1;
}


int test_rio_readlineb(char *filename)
{
    rio_t  rt;
    rio_t *rp  = &rt;
    int    fd  = -1;
    int    max = 1024;
    char buf[MAX_LEN];

    memset(buf, 0, MAX_LEN);

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: fail to open %s\n", filename);
        return -1;
    }

    rio_readinitb(rp, fd);

    while (rio_readlineb(rp, buf, max) > 0) {
        printf("Readlineb: %s\n", buf);
    }

    close(fd);
    return 0;
}


int main(size_t argc, void **argv)
{
    if (argc != 2) {
        printf("Error: input filename");
        return 0;
    }

    int res = 0;

    test_rio_readlineb(argv[1]);

    test_rio_readn(argv[1]);

    test_rio_read_and_rio_write(argv[1]);

    test_rio_readnb(argv[1]);

    return 0;
}
