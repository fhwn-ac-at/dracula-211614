#include "distribution.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

distribution_info_t distribution_infos[DISTRIBUTION_COUNT] = {
    { 0  , 0         },
    { 'u', "uniform" }
};

distribution_t strtodistr(const char* str) {
    if (!str)
        return DISTR_NONE;
    bool isshort = str[0] != '\0' && str[1] == '\0';
    for (size_t i = 1; i < DISTRIBUTION_COUNT; i++)
        if ((isshort && str[0] == distribution_infos[i].shortname) || strcmp(str, distribution_infos[i].longname) == 0)
            return i;
    return DISTR_NONE;
}
