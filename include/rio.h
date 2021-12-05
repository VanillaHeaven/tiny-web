#ifndef RIO_H
#define RIO_H
#define RIO_BUF_SIZE 8096
#include <errno.h>
#include <string.h>

typedef struct rio_s rio_t;

struct rio_s {
    int rio_fd;
    int rio_remain;
    char *rio_bufp;
    char rio_buf[RIO_BUF_SIZE];
};

ssize_t rio_readn(int fd, void *buf, size_t n);
ssize_t rio_writen(int fd, void *buf, size_t n);
ssize_t rio_readinitb(rio_t *rp, int fd);
ssize_t rio_readnb(rio_t *rp, void *userbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *userbuf, size_t max_len);

#endif // DEBUG