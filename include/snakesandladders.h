#pragma once

#include "array.h"

/**
 * Struct for a snake or ladder.
 */
typedef struct snakeorladder_t {
    size_t src;       // The index of the starting cell
    size_t dst;       // The index of the ending cell
} snakeorladder_t;

/**
 * Checks whether the given snakeorladder_t is a snake (a > b).
 * @param sol The snakeorladder_t to check.
 * @return true if it is a snake (a > b), false otherwise or if no sol was given.
 */
bool issnake(snakeorladder_t* sol);

/**
 * Checks whether the given snakeorladder_t is a ladder (a < b).
 * @param sol The snakeorladder_t to check.
 * @return true if it is a ladder (a < b), false otherwise or if no sol was given.
 */
bool isladder(snakeorladder_t* sol);

/**
 * Parses the given string to a snakeorladder_t. The string must consist of two positive integer values > 0 separated by a hyphen '-'.
 * Format a-b where a and b are positive integer values > 0.
 * @param str The string that should be parsed.
 * @param error The address where the error code should be stored if it is given
 * -  0 successfully parsed string
 * -  1 no string given
 * -  2 unable to duplicate the given string
 * -  3 string does not contain the split character '-'
 * -  4 value a out of range (overflow/underflow)
 * -  5 invalid characters in a (not a number)
 * -  6 remaining characters after number in string for a
 * -  7 a is 0
 * -  8 value b out of range (overflow/underflow)
 * -  9 invalid characters in b (not a number)
 * - 10 remaining characters after number in string for b
 * - 11 b is 0
 * @return The parsed snakeorladder_t. If the string could not be parsed (snakeorladder_t){.a=0,.b=0 } is returned and the error code is set in error.
 */
snakeorladder_t strtosol(const char* str, int* error);
