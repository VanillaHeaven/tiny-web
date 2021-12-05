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


int parse(int conn_fd, request_t* r)
{
    int        cflags          = REG_EXTENDED | REG_ICASE;
    size_t     sub_num_reqline = 5;
    size_t     sub_num_header  = 2;
    char*      pattern_reqline = "^([a-zA-Z]+) (https?://([^/]*))?(/[^ ]*) HTTP/(1.1|1.0)$";
    char*      pattern_header  = "^([^:]+): *(.*)$";
    regmatch_t sub_exp_reqline[5];
    regmatch_t sub_exp_header[2];
    regex_t    reg_reqline;
    regex_t    reg_header;

    if (regcomp(&reg_reqline, pattern_reqline, cflags)) {
        return -1;
    }

    if (regcomp(&reg_header, pattern_header, cflags)) {
        return -1;
    }
    
    int   status = MATCH_INIT;
    int   code   = PARSE_FAIL;
    int   conti  = 1;
    int   ret    = -1;

    char  line[LINEMAXLEN];
    memset(line, 0, LINEMAXLEN);

    rio_t rt;
    rio_readinitb(&rt, conn_fd);

    while (conti && rio_readlineb(&rt, line, LINEMAXLEN) > 0) {
        switch(status)
        {
            case MATCH_INIT:
                ret = regexec(&reg_reqline, line, sub_num_reqline, sub_exp_reqline, 0);
                if (ret == 0) {
                    status = MATCH_REQ_LINE;
                    printf("[TEST] request_line: %s\n", line);
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
                    printf("[TEST] header_line: %s\n", line);
                    // parse header line;
                } else if (strcmp(line, "\r\n") == 0 ||
                           strcmp(line, "\n\r") == 0 ||
                           strcmp(line, "\n") == 0 ||
                           strcmp(line, "\r") == 0 )
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
    }

    regfree(&reg_reqline);
    regfree(&reg_header);

    if (code == PARSE_FAIL) {
        // free member;
    }

    return code;
}
