#include "die.h"

#include "tsrand48.h"

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
