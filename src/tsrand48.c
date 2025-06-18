#include "tsrand48.h"

#include <stdlib.h>
#include <threads.h>

// thread local random number generator seed
static thread_local tsseed48_t seed = {};

tsseed48_t tsseed48(const tsseed48_t* newseed) {
    tsseed48_t seed_prev = seed;
    seed = *newseed;
    return seed_prev;
}

tsseed48_t tsnewseed48() {
    tsseed48_t seed_prev = seed;
    uint64_t newseed = time(0) ^ (uint64_t)thrd_current();
    seed = (tsseed48_t){
        (newseed >> 16) & 0xffff,
        newseed & 0xffff,
        (newseed >> 32) & 0xffff
    };
    return seed_prev;
}

double tserand48() {
    return erand48(seed.xsubi);
}

long tsnrand48() {
    return nrand48(seed.xsubi);
}

long tsjrand48() {
    return jrand48(seed.xsubi);
}
