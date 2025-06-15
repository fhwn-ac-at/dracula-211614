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
 * Struct containing an array of snakesorladder_ts.
 */
typedef struct snakesandladders_t {
    array_t array;
} snakesandladders_t;

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
 * Parses the given string to a snakeorladder_t. The string must consist of two positive integer values separated by a '-' character.
 * Format a-b where a is a and b are positive integer values.
 * @param str The string that should be parsed.
 * @param error The address where the error code should be stored if it is given
 * - 0 successfully parsed string
 * - 1 no string given
 * - 2 unable to duplicate the given string
 * - 3 string does not contain the split character '-'
 * - 4 value a out of range (overflow/underflow)
 * - 5 invalid characters in a (not a number)
 * - 6 remaining characters after number in string for a
 * - 7 value b out of range (overflow/underflow)
 * - 8 invalid characters in b (not a number)
 * - 9 remaining characters after number in string for b
 * @return The parsed snakeorladder_t. If the string could not be parsed (snakeorladder_t){.a=0,.b=0 } is returned and the error code is set in error.
 */
snakeorladder_t strtosol(const char* str, int* error);

/**
 * Creates a new snakesandladders_t containing an empty array with the given initial capacity and elementsize = sizeof(snakeorladder_t).
 * @param initcapacity The initial capacity of the snakesandladders_t array.
 * @return The newly created snakesandladders_t instance.
 */
snakesandladders_t sals_create(size_t initcapacity);

/**
 * Frees the given snakesandladders_t by freeing it's array with the array_free function.
 * @param sals The snakesandladders_t instance that should be freed.
 */
void sals_free(snakesandladders_t* sals);

/**
 * Removes all elements from the snakesandladders_t by clearing it's array with the array_clear function.
 * @param sals The snakesandladders_t instance that should be cleared.
 */
void sals_clear(snakesandladders_t* sals);

/**
 * Adds the given snakeorladder_t to the end of the snakesandladders_t's array with the array_add function.
 * If the element could not be added no action is performed and 0 is returned.
 * @param sals The snakesandladders_t instance sol should be added to.
 * @param sol The snakeorladder_t instance that should be added to sals.
 * @return The address of the newly added element, 0 if it could not be added.
 */
snakeorladder_t* sals_add(snakesandladders_t* sals, const snakeorladder_t* sol);

/**
 * Retrieves the address of the snakeorladder_t at the given index in the snakesandladders_t's array.
 * @return The address of the snakeorlatter_t at the given index.
 */
snakeorladder_t* sals_get(snakesandladders_t* sals, size_t index);
