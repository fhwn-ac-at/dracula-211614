#include "snakesandladders.h"

#include "str.h"
#include "parsenum.h"

#include <stdlib.h>

bool issnake(snakeorladder_t* sol) {
    return sol ? sol->a > sol->b : false;
}

bool isladder(snakeorladder_t* sol) {
    return sol ? sol->a < sol->b : false;
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
    int err = parse_uint64(twostrs.strs[0], &sol.a);
    if (err != 0) {
        if (error)
            *error = err + 2;
        free(strdup);
        return (snakeorladder_t){};
    }
    err = parse_uint64(twostrs.strs[1], &sol.b);
    if (err != 0) {
        if (error)
            *error = err + 5;
        free(strdup);
        return (snakeorladder_t){};
    }
    if (error)
        *error = 0;
    free(strdup);
    return sol;
}

snakesandladders_t sals_create(size_t initcapacity) {
    return (snakesandladders_t){ array_create(initcapacity, sizeof(snakeorladder_t)) };
}

void sals_free(snakesandladders_t* sals) {
    if (!sals)
        return;
    array_free(&sals->array);
}

void sals_clear(snakesandladders_t* sals) {
    if (!sals)
        return;
    array_clear(&sals->array);
}

snakeorladder_t* sals_add(snakesandladders_t* sals, const snakeorladder_t* sol) {
    return sals ? array_add(&sals->array, sol) : 0;
}
