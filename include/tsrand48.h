#pragma once

/**
 * Thread-safe rand48 random number generator
 */

#include <stdint.h>

/**
 * Struct to store a seed value for the rand48 number generator
 */
typedef struct tsseed48_t {
    uint16_t xsubi[3];          // The seed value
} tsseed48_t;

/**
 * Sets the thread local internal seed of the random number generator to the given seed.
 * @param newseed The seed the random number generator should be seeded with.
 * @return A copy of the previous seed.
 */
tsseed48_t tsseed48(const tsseed48_t* newseed);

/**
 * Sets the thread local internal seed of the random number generator
 * to a new value that is the current time XOR the thread id of the executing thread.
 * @return A copy of the previous seed.
 */
tsseed48_t tsnewseed48();

/**
 * Thread-safely generates a pseudo-random nonnegative double-precision floating-point value uniformly distributed over the interval [0.0, 1.0).
 * @return The pseudo-randomly generated double-precision floating-point value.
 */
double tserand48();

/**
 * Thread-safely generates a pseudo-random nonnegative long integer uniformly distributed over the interval [0, 2^31).
 * @return The pseudo-randomly generated long integer.
 */
long tsnrand48();

/**
 * Thread-safely generates a pseudo-random signed long integers uniformly distributed over the interval [-2^31, 2^31).
 * @return The pseudo-randomly generated long integer.
 */
long tsjrand48();
