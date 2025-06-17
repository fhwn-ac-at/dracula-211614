#include "distribution.h"

#include "str.h"
#include "numbers.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

distr_preset_info_t distr_preset_infos[DISTR_PRESET_COUNT] = {
    { 0         },
    { "uniform" },
    { "twodice" }
};

distribution_t distr_create_empty() {
    return distr_create(DISTR_PRESET_NONE);
}

distribution_t distr_create(distr_preset_t preset) {
    return (distribution_t){ preset, array_create(0, sizeof(size_t), 0) };
}

bool distr_isempty(const distribution_t* distr) {
    return distr ? distr->preset == DISTR_PRESET_NONE && array_isempty(&distr->weights) : true;
}

void distr_free(distribution_t* distr) {
    if (!distr)
        return;
    array_free(&distr->weights);
    *distr = distr_create_empty();
}

distribution_t strtodistr(const char* str, int* error) {
    if (!str) {
        if (error)
            *error = 1;
        return distr_create_empty();
    }
    if (error)
        *error = 0;

    // check if string represents a distribution preset
    for (size_t i = 1; i < DISTR_PRESET_COUNT; i++)
        if (strcmp(str, distr_preset_infos[i].name) == 0)
            return distr_create(i);

    // convert string to distribution weights
    distribution_t distr = distr_create_empty();
    char* strdup = strduplicate(str);
    if (!strdup) {
        if (error)
            *error = 2;
        return distr_create_empty();
    }
    twostrs_t twostrs = (twostrs_t){ 0, strdup };
    do {
        twostrs = strsplitnext(twostrs.strs[1], ',');
        size_t weight = 0;
        int err = strtouint64(twostrs.strs[0], &weight);
        if (err) {
            if (error)
                *error = err + 1;
            free(strdup);
            distr_free(&distr);
            return distr_create_empty();
        }
        if (!array_add(&distr.weights, &weight)) {
            if (error)
                *error = 7;
            free(strdup);
            distr_free(&distr);
            return distr_create_empty();
        }
    } while (twostrs.strs[1]);

    return distr;
}

int distr_build(distribution_t* distr, size_t die_sides) {
    if (!distr)
        return 1;
    switch (distr->preset) {
        case DISTR_PRESET_NONE:
        {
            if (distr->weights.size != die_sides) {
                // check if to many distribution weights where specified
                if (distr->weights.size > die_sides)
                    return 3;
                // add missing distribution weights
                size_t weight = 0;
                for (size_t i = distr->weights.size; i < die_sides; i++) {
                    if (!array_add(&distr->weights, &weight)) {
                        distr_free(distr);
                        return 4;
                    }
                }
            }
            break;
        }
        case DISTR_PRESET_UNIFORM:
        {
            distr_free(distr);
            int error = 0;
            *distr = distr_preset_build_uniform(die_sides, &error);
            if (error)
                return error + 3;
            break;
        }
        case DISTR_PRESET_TWODICE:
        {
            distr_free(distr);
            int error = 0;
            *distr = distr_preset_build_twodice(die_sides, &error);
            if (error)
                return error + 3;
            break;
        }
        default:
            return 3;
    }
    return 0;
}

distribution_t distr_preset_build_uniform(size_t die_sides, int* error) {
    distribution_t distr = distr_create(DISTR_PRESET_UNIFORM);
    size_t weight = 1;
    for (size_t i = 0; i < die_sides; i++) {
        if (!array_add(&distr.weights, &weight)) {
            if (error)
                *error = 1;
            distr_free(&distr);
            return distr_create_empty();
        }
    }
    return distr;
}

distribution_t distr_preset_build_twodice(size_t die_sides, int* error) {
    if (die_sides == 0) {
        if (error)
            *error = 2;
        return distr_create_empty();
    }
    if (die_sides % 2 != 0) {
        if (error)
            *error = 3;
        return distr_create_empty();
    }
    distribution_t distr = distr_create(DISTR_PRESET_TWODICE);
    size_t weight = 0;
    for (; weight < die_sides / 2; weight++) {
        if (!array_add(&distr.weights, &weight)) {
            if (error)
                *error = 1;
            distr_free(&distr);
            return distr_create_empty();
        }
    }
    for (; weight != 0; weight--) {
        if (!array_add(&distr.weights, &weight)) {
            if (error)
                *error = 1;
            distr_free(&distr);
            return distr_create_empty();
        }
    }
    return distr;
}
