#include "distribution.h"

static distribution_t distribution = DISTR_UNIFORM;

distribution_t distribution_set(distribution_t newdist) {
    distribution_t prevdist = distribution;
    distribution = newdist;
    return prevdist;
}

distribution_t distribution_get() {
    return distribution;
}
