#pragma once

#include "distribution.h"

/**
 * Struct for a die.
 */
typedef struct die_t {
    array_t sides;          // probabilities for all sides of the die (element type: double)
} die_t;

/**
 * Creates an empty die.
 * @return The created die.
 */
die_t die_create_empty();

/**
 * Creates a die with the given distribution.
 * @param distr The distribution that should be used.
 * @return The created die.
 */
die_t die_create(const distribution_t* distr);

/**
 * Frees the given die freeing it's array of side probabilities.
 * @param die The die that should be freed.
 */
void die_free(die_t* die);

/**
 * Checks wether the given die is empty meaning it's array of sides has size = 0.
 * @param die The die that should be checked.
 * @return true if the die is empty or none was given, false otherwise.
 */
bool die_isempty(const die_t* die);

/**
 * Dices the given die thread-safely generating a random die side with the distribution according to the die's side probabilities.
 * Possible side values lie in the interval [1, die->sides.size] (1 based indexing meaning the first side of the die has index 1, not 0).
 * @param die The die that should be diced.
 * @return The diced side.
 */
size_t dice(const die_t* die);

/**
 * Uses the given die the given number of times and prints a summary of the diced sides.
 * @param die The die that should be used to dice.
 * @param iterations The number of times the die should be diced.
 */
void simulate_dices(const die_t* die, size_t iterations);

/**
 * Prints the given die.
 * @param die The die that should be printed.
 * @param barlength The length of the longest printed bar.
 */
void die_print(const die_t* die, size_t barlength);
