#include "snakesandladders.h"

#include "str.h"
#include "numbers.h"

#include <stdlib.h>

bool issnake(snakeorladder_t* sol) {
    return sol ? sol->src > sol->dst : false;
}

bool isladder(snakeorladder_t* sol) {
    return sol ? sol->src < sol->dst : false;
}

snakeorladder_t strtosol(const char* str, int* error) {
    if (!str) {
        if (error)
            *error = 1;
        return (snakeorladder_t){};
    }
    char* strdup = strduplicate(str);
    if (!strdup) {
        if (error)
            *error = 2;
        return (snakeorladder_t){};
    }
    twostrs_t twostrs = strsplitnext(strdup, '-');
    if (!twostrs.strs[1]) {
        if (error)
            *error = 3;
        free(strdup);
        return (snakeorladder_t){};
    }
    snakeorladder_t sol = {};
    int err = strtouint64(twostrs.strs[0], &sol.src);
    if (err != 0) {
        if (error)
            *error = err + 2;
        free(strdup);
        return (snakeorladder_t){};
    }
    if (sol.src == 0) {
        if (error)
            *error = 7;
        free(strdup);
        return (snakeorladder_t){};
    }
    err = strtouint64(twostrs.strs[1], &sol.dst);
    if (err != 0) {
        if (error)
            *error = err + 6;
        free(strdup);
        return (snakeorladder_t){};
    }
    if (sol.dst == 0) {
        if (error)
            *error = 11;
        free(strdup);
        return (snakeorladder_t){};
    }
    if (error)
        *error = 0;
    free(strdup);
    return sol;
}
