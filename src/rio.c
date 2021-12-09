#include <stdio.h>
#include "rio.h"


ssize_t rio_readn(int fd, void *buf, size_t n)
{
    if (fd < 0 || buf == NULL || n <= 0){
        return -1;
    }

    size_t   ch_remain = n;
    ssize_t  ch_read   = 0;
    char    *buf_p     = buf;
    errno = 0;

    while (ch_remain > 0)
    {
        ch_read = read(fd, buf_p, ch_remain);
        if (ch_read < 0) {
            if (errno == EINTR)
                ch_read = 0;
            else
                return -1;
        }
        else if (ch_read == 0)
            break;

        ch_remain -= ch_read;
        buf_p += ch_read;
    }

    return n - ch_remain;
}


ssize_t rio_writen(int fd, void *buf, size_t n)
{
    if (fd < 0 || buf == NULL || n <= 0){
        return -1;
    }

    size_t ch_remain = n;
    ssize_t ch_wrote = 0;
    char *buf_p = buf;
    errno = 0;

    while (ch_remain > 0)
    {
        ch_wrote = write(fd, buf_p, ch_remain);
        if (ch_wrote <= 0) {
            if (errno == EINTR)
                ch_wrote = 0;
            else
                return -1;
        }

        ch_remain -= ch_wrote;
        buf_p += ch_wrote;
    }

    return n - ch_remain;
}


static ssize_t rio_read(rio_t *rp, void *buf, size_t n)
{
    // 当缓冲区为空，读入 n 个字节的内容（这个能异步完成吗？）
    while (rp->rio_remain <= 0) {
        rp->rio_remain = read(rp->rio_fd, rp->rio_buf, RIO_BUF_SIZE);
        if (rp->rio_remain < 0) {
            if (errno != EINTR)
                return -1;
        }
        else if (rp->rio_remain == 0)
            return 0;
        else {
            rp->rio_bufp = rp->rio_buf;
        }
    }

    int n_read;
    n_read = rp->rio_remain < n ? rp->rio_remain : n;
    memcpy(buf, rp->rio_bufp, n_read);

    rp->rio_remain -= n_read;
    rp->rio_bufp += n_read;

    return n_read;
}


ssize_t rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd     = fd;
    rp->rio_remain = 0;
    rp->rio_bufp   = rp->rio_buf;
}


ssize_t rio_readnb(rio_t *rp, void *userbuf, size_t n)
{
    size_t   ch_remain = n;
    ssize_t  ch_read   = 0;
    char    *buf_p     = userbuf;

    while (ch_remain) {
        ch_read = rio_read(rp, buf_p, ch_remain);
        if (ch_read < 0)
            return -1;
        else if (ch_read == 0)
            break;
        ch_remain -= ch_read;
        buf_p += ch_read;
    }

    return n - ch_remain;
}


ssize_t rio_readlineb(rio_t *rp, void *userbuf, size_t max_len)
{
    // 一次读一个字符
    int   status = 1;
    int   line_len = 0;
    char *bufp = userbuf;

    while (line_len <= max_len) {
        status = rio_read(rp, bufp, 1);

        if (status == 0) {
            *bufp = 0;
            return line_len;
        }
        else if (status < 0)
            return -1;

        line_len++;
        if (*bufp == '\n') {
            bufp++;
            *bufp = 0;
            return line_len;
        }
        bufp++;
    }

    return -1;
}