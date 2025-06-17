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
    err = strtouint64(twostrs.strs[1], &sol.dst);
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
    return (snakesandladders_t){ array_create(initcapacity, sizeof(snakeorladder_t), 0) };
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

snakeorladder_t* sals_get(snakesandladders_t* sals, size_t index) {
    return sals ? array_get(&sals->array, index) : 0;
}
