#ifndef REQUEST_H
#define REQUEST_H

#include <string.h>

typedef struct request_s request_t;

struct header_node
{
    char* key;
    char* data;
    struct header_node* next;
};

struct request_s
{
    char* uri;
    char* method;
    char* proto;
    struct header_node* headers_in;
};


int parse(int, request_t*);

#endif