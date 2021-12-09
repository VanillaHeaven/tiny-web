#include <sys/socket.h>
#include <netinet/in.h>
#include "tinyweb.h"


int get_listen_fd(int port);
void echo(int conn_fd);


int main(int argc, char** argv) {

    int port = 0;
    int listen_fd, conn_fd;
    int client_addr_len;
    struct sockaddr_storage client_addr;

    // 1. 监听一个端口
    char *ch = argv[1];

    while (*ch != '\0') {
        port = port * 10 + (*ch - 48);
        ch++;
    }

    printf("[TINY_WEB] listening on: %d\n", port);

    listen_fd = get_listen_fd(port);
    if (listen_fd == -1) {
        printf("Error: fail to listen port %d\n", port);
        return -1;
    }

    // 2. 接收请求
    client_addr_len = sizeof(client_addr);
    while(1) {
        memset(&client_addr, 0, client_addr_len);
        conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        // 3. 处理请求
        echo(conn_fd);
        // 4. 关闭连接
        close(conn_fd);
    }

    printf("tiny web listening on port %d.\n", port);
    return 0;
}


int get_listen_fd(int port) {

    int fd;
    struct sockaddr_in sa;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_port        = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }

    if (bind(fd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        return -1;
    }

    if(listen(fd, 1024) == -1) {
        return -1;
    }

    return fd;
}


void echo(int conn_fd) {
    printf("connected success, connect fd is %d.\n", conn_fd);
    request_t r;
    int res = request_parse(conn_fd, &r);
    if (res == 0) {
        printf("[TEST] parse succ\n");
        printf("[TEST] method: %s\n", r.method);
        printf("[TEST] scheme: %s\n", r.scheme);
        printf("[TEST] host: %s\n", r.host);
        printf("[TEST] uri: %s\n", r.uri);
        printf("[TEST] proto: %s\n", r.proto);
        
        header_node_t *h = r.headers_in;
        while (h != NULL) {
            printf("[TEST] %s: %s\n", h->key, h->data);
            h = h->next;
        }
    }
    else {
        printf("parse fail.");
    }
    return;
}