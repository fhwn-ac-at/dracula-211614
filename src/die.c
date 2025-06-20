#include "die.h"

#include "cvts.h"
#include "tsrand48.h"

#include <stdio.h>
#include <stdlib.h>

die_t die_create_empty() {
    return (die_t){ array_create(0, sizeof(double), 0) };
}

die_t die_create(const distribution_t* distr) {
    if (!distr)
        return die_create_empty();
    die_t die = { array_create(distr->weights.size, sizeof(double), 0) };
    if (!die.sides.data)
        die_create_empty();
    // calculate probabilities from weights
    size_t weightsum = 0;
    for (size_t i = 0; i < distr->weights.size; i++)
        weightsum += *(size_t*)array_getconst(&distr->weights, i);
    for (size_t i = 0; i < distr->weights.size; i++)
        array_add(&die.sides, &(double){ *(size_t*)array_getconst(&distr->weights, i) / (double)weightsum });
    return die;
}

void die_free(die_t* die) {
    if (!die)
        return;
    array_free(&die->sides, 0);
    *die = (die_t){};
}

bool die_isempty(const die_t* die) {
    return die ? die->sides.size == 0 : false;
}

size_t dice(const die_t* die) {
    // thread-safely generate random double in interval [0,1) 
    double random = tserand48();
    // apply die side probabilities (distribution)
    double offset = 0.0;
    size_t side = 0;
    for (; side < die->sides.size; side++) {
        offset += *(double*)array_getconst(&die->sides, side);
        if (random < offset)
            return side + 1;
    }
    return side;
}

void simulate_dices(const die_t* die, size_t iterations) {
    printf("\n");
    size_t* sides = malloc(die->sides.size * sizeof(*sides));
    if (!sides) {
        fprintf(stderr, "%serror:%s unable to simulate %lu dices.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), iterations);
        exit(1);
    }
    for (size_t side = 1; side != die->sides.size + 1; side++)
        sides[side - 1] = 0;
    printf("simulating %lu dices\n", iterations);
    tsnewseed48();
    size_t it = 0;
    for (; it < iterations; it++)
        sides[dice(die) - 1]++;
    for (size_t side = 1; side != die->sides.size + 1; side++)
        printf("%s%3lu%s %12lu %10.6lf%%\n", FMT(FMTVAL_FG_BRIGHT_BLACK), side, FMT(FMTVAL_FG_DEFAULT), sides[side - 1], ((double)sides[side - 1] / (double)iterations) * 100.0);
    printf("\n");
    free(sides);

}

void die_print(const die_t* die, size_t barlength) {
    if (!die)
        return;

    // setup colors
    typedef struct color_t {
        const char* fg;
        const char* bg;
    } color_t;
    const color_t colors[] = {
        { FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_BG_BRIGHT_BLACK)    },
        { FMT(FMTVAL_FG_WHITE)       , FMT(FMTVAL_BG_WHITE)           }
    };
    size_t colorcount = sizeof(colors) / sizeof(*colors);
    size_t color = 0;

    // determine barlen multiplier to streach the longest bar to be barlength characters long
    double maxprob = 0;
    for (size_t side = 0; side < die->sides.size; side++) {
        double prob = *(const double*)array_getconst(&die->sides, side);
        if (maxprob < prob)
            maxprob = prob;
    }
    double barstretch = maxprob != 0.0 ? barlength / maxprob : 1.0;

    // print die
    printf("\n%lu-sided die\n", die->sides.size);
    for (size_t side = 0; side < die->sides.size; side++) {
        double prob = *(const double*)array_getconst(&die->sides, side);
        printf("%s%3lu %7.3lf%%%s %s%*s%s\n", colors[color].fg, side + 1, prob * 100.0, FMT(FMTVAL_FG_DEFAULT), colors[color].bg, (int)(prob * barstretch), "", FMT(FMTVAL_BG_DEFAULT));
        color = (color + 1) % colorcount;
    }
    printf("\n");
}
