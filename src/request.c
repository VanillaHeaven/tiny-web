#include <regex.h>
#include <stdio.h>
#include "rio.h"
#include "request.h"

#define LINEMAXLEN 4096
#define MATCH_INIT -1
#define MATCH_REQ_LINE 2
#define MATCH_HDR_LINE 3

#define PARSE_SUCC 0
#define PARSE_FAIL -1


int request_parse(int conn_fd, request_t *r)
{
    int        cflags          = REG_EXTENDED | REG_ICASE;
    size_t     sub_num_reqline = 6;
    size_t     sub_num_header  = 3;
    char*      pattern_reqline = "^([a-zA-Z]+) (https?://([^/]*))?(/[^ ]*) (HTTP/(1.1|1.0))";
    char*      pattern_header  = "^([^:]+): *([^\r\n]+)";
    regmatch_t sub_exp_reqline[6];
    regmatch_t sub_exp_header[3];
    regex_t    reg_reqline;
    regex_t    reg_header;

    if (regcomp(&reg_reqline, pattern_reqline, cflags)) {
        return PARSE_FAIL;
    }

    if (regcomp(&reg_header, pattern_header, cflags)) {
        return PARSE_FAIL;
    }
    
    int   status = MATCH_INIT;
    int   code   = PARSE_FAIL;
    int   conti  = 1;
    int   ret    = -1;

    char  line[LINEMAXLEN];
    memset(line, 0, LINEMAXLEN);

    rio_t rt;
    rio_readinitb(&rt, conn_fd);
    header_node_t *pre_h = NULL;

    while (conti && rio_readlineb(&rt, line, LINEMAXLEN) > 0) {
        switch(status)
        {
            case MATCH_INIT:
                ret = regexec(&reg_reqline, line, sub_num_reqline, sub_exp_reqline, 0);
                if (ret == 0) {
                    status = MATCH_REQ_LINE;
                    r->method = strndup(line + sub_exp_reqline[1].rm_so,
                        sub_exp_reqline[1].rm_eo - sub_exp_reqline[1].rm_so);
                    r->scheme = strndup(line + sub_exp_reqline[2].rm_so,
                        sub_exp_reqline[2].rm_eo - sub_exp_reqline[2].rm_so);
                    r->host   = strndup(line + sub_exp_reqline[3].rm_so,
                        sub_exp_reqline[3].rm_eo - sub_exp_reqline[3].rm_so);
                    r->uri    = strndup(line + sub_exp_reqline[4].rm_so,
                        sub_exp_reqline[4].rm_eo - sub_exp_reqline[4].rm_so);
                    r->proto  = strndup(line + sub_exp_reqline[5].rm_so,
                        sub_exp_reqline[5].rm_eo - sub_exp_reqline[5].rm_so);
                } else {
                    code = PARSE_FAIL;
                    conti = 0;
                }
                break;

            case MATCH_REQ_LINE:
            case MATCH_HDR_LINE:
                ret = regexec(&reg_header, line, sub_num_header, sub_exp_header, 0);
                if (ret == 0) {
                    status = MATCH_HDR_LINE;
                    header_node_t *h = (header_node_t *) malloc(sizeof(header_node_t));
                    h->key  = strndup(line + sub_exp_header[1].rm_so,
                        sub_exp_header[1].rm_eo - sub_exp_header[1].rm_so);
                    h->data = strndup(line + sub_exp_header[2].rm_so,
                        sub_exp_header[2].rm_eo - sub_exp_header[2].rm_so);
                    h->next = pre_h;
                    pre_h   = h;
                    r->headers_in = pre_h;
                } else if (strcmp(line, "\r\n") == 0)
                {
                    code = PARSE_SUCC;
                    conti = 0;
                } else {
                    code = PARSE_FAIL;
                    conti = 0;
                }
                break;

            default:
                code = PARSE_FAIL;
                conti = 0;
        }
        memset(line, 0, LINEMAXLEN);
    }

    regfree(&reg_reqline);
    regfree(&reg_header);

    if (code == PARSE_FAIL) {
        // free member;
    }

    printf("==== parse end ====\n");
    return code;
}
