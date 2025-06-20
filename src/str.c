#include "str.h"

#include <string.h>
#include <stdlib.h>

twostrs_t strsplitnext(char* str, char splitchar) {
    twostrs_t twostrs = { { (char*)str, 0 } };
    if (!str)
        return twostrs;
    for (; *str && *str != splitchar; str++);
    if (*str == '\0')
        return twostrs;
    twostrs.strs[1] = (char*)str + 1;
    if (twostrs.strs[0] && twostrs.strs[1] && twostrs.strs[1] - twostrs.strs[0] > 0)
        *(twostrs.strs[1] - 1) = '\0';
    return twostrs;
}

char* strduplicate(const char* str) {
    if (!str)
        return 0;
    char* newstr = malloc(strlen(str) + 1);
    if (!newstr)
        return 0;
    return strcpy(newstr, str);
}

bool iswhitespace(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\f':
        case '\v':
            return true;
        default:
            return false;
    }
}
