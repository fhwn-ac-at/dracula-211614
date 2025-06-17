#include "numbers.h"

#include <errno.h>
#include <stdlib.h>

int strtodouble(const char *str, double* value) {
    if (!str)
        return 1;
    char *endptr;
    errno = 0;
    double val = strtod(str, &endptr);
    if (errno == ERANGE)
        return 2;
    if (endptr == str)
        return 3;
    if (*endptr != '\0')
        return 4;
    if (value)
        *value = val;
    return 0;
}

int strtouint64(const char *str, uint64_t* value) {
    if (!str)
        return 1;
    char *endptr;
    errno = 0;
    uint64_t val = strtoul(str, &endptr, 10);
    if (errno == ERANGE)
        return 2;
    if (endptr == str)
        return 3;
    if (*endptr != '\0')
        return 4;
    if (value)
        *value = val;
    return 0;
}
