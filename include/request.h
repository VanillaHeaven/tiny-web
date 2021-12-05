#ifndef REQUEST_H
#define REQUEST_H

#include <string.h>

typedef struct request_s request_t;
typedef struct header_node_s header_node_t;

struct header_node_s
{
    char* key;
    char* data;
    struct header_node_s *next;
};

struct request_s
{
    char *uri;
    char *method;
    char *proto;
    char *scheme;
    char *host;
    header_node_t *headers_in;
};


int request_parse(int, request_t*);

#endif