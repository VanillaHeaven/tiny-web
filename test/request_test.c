#include <stdio.h>
#include "tinyweb.h"


int main(int argc, void **argv)
{
    if (argc != 2) {
        printf("Error: input filename");
        return 0;
    }

    int   fd       = -1;
    char *filename = argv[1];

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: fail to open %s\n", filename);
        return -1;
    }

    request_t r;
    int ret = request_parse(fd, &r);
    if (ret == 0) {
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
        printf("[TEST] parse fail\n");
    }

    return 0;
}
