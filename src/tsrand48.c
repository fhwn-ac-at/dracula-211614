#include "tsrand48.h"

#include <pthread.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

// thread local random number generator state
static thread_local unsigned short xsubi[3] = {};

// The previous value of the thread local xsubi
static thread_local unsigned short xsubi_prev[3] = {};

const unsigned short* tsseed48(unsigned short seed16v[3]) {
    for (int i = 0; i < 3; i++)
        xsubi_prev[i] = xsubi[i];
    for (int i = 0; i < 3; i++)
        xsubi[i] = seed16v[i];
    return xsubi_prev;
}

const unsigned short* tsnewseed48() {
    for (int i = 0; i < 3; i++)
        xsubi_prev[i] = xsubi[i];
    unsigned long seed = time(0) ^ (unsigned long)pthread_self();
    xsubi[0] = (seed >> 16) & 0xFFFF;
    xsubi[1] = seed & 0xFFFF;
    xsubi[2] = (seed >> 32) & 0xFFFF;
    return xsubi_prev;
}

double tserand48() {
    return erand48(xsubi);
}

long tsnrand48() {
    return nrand48(xsubi);
}

long tsjrand48() {
    return jrand48(xsubi);
}
