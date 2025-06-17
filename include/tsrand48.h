#pragma once

/**
 * Thread-safe rand48 random number generator
 */

/**
 * Sets the thread local internal random number generator state xsubi to the given array of 3 unsigned shorts seed16v.
 * The previous value of xsubi is stored in an internal buffer and it's address is returned.
 * The internal buffer content is overwritten every time the tsseed48 or tsnewseed48 function is called.
 * @param seed16v The value the random number generator state xsubi should be set to.
 * @return The address of an internal buffer containing the previous value of xsubi.
 */
const unsigned short* tsseed48(unsigned short seed16v[3]);

/**
 * Sets the thread local internal random number generator state xsubi to a new value that is the current time XOR the thread id.
 * The previous value of xsubi is stored in an internal buffer and it's address is returned.
 * The internal buffer content is overwritten every time the tsseed48 or tsnewseed48 function is called.
 * @return The address of an internal buffer containing the previous value of xsubi.
 */
const unsigned short* tsnewseed48();

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
