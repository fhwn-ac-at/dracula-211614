#pragma once

#include "simulator.h"

#include <stddef.h>

/**
 * The summary statistics about a set of unsigned integer values
 * containing the sum, minimum, maximum and average.
 */
typedef struct valstats_t {
    size_t sum;                     // The sum of the statistically analyzed values
    size_t min;                     // The smallest out of the statistically analyzed values
    size_t max;                     // The largest out of the statistically analyzed values
    double avg;                     // The average out of the statistically analyzed values
} valstats_t;

/**
 * The statistics about a snake or ladder.
 */
typedef struct solstats_t {
    snakeorladder_t sol;            // The snake or ladder these statistics are about
    valstats_t uses;                // The summary statistics about the usage of this snake or ladder
} solstats_t;

/**
 * Struct to store the statistics collected by many game simulations.
 */
typedef struct stats_t {
    size_t sims;                    // The number of run simulations
    size_t wins;                    // The number of won games
    size_t losses;                  // The number of lost games (forfeited due to reaching the dice limit without winning)
    double winrate;                 // The relative number of wins (wins / sims)
    double lossrate;                // The relative number of losses (losses / sims)
    size_t dicelimit;               // The maximum allowed number of dices in a simulation before resigning if the game wasn't won yet.
    valstats_t dices;               // The summary statistics about the dices in all simulations
    array_t shortestdices;          // The shortest dice sequence to lead to a win out of all simulations (element type: size_t)
    valstats_t salsuses;            // The summary statistics about the number of used snakes and ladders in all simulations.
    valstats_t snakesuses;          // The summary statistics about the number of used snakes in all simulations.
    valstats_t laddersuses;         // The summary statistics about the number of used ladders in all simulations.
    double snakesuserate;           // The relative number of snake uses out of all snakes and ladders uses.
    double laddersuserate;          // The relative number of ladders uses out of all snakes and ladders uses.
    array_t sals;                   // Array of summary statistics about each individual snake or ladder in all simulations (element type: solstats_t)
} stats_t;

/**
 * Creates empty statistics ready to be used for simulations analysis.
 * @return The created statistics.
 */
stats_t stats_create();

/**
 * Frees the given statistics freeing it's shortestdices and snakes and ladders arrays.
 * @param stats The stats that should be freed.
 */
void stats_free(stats_t* stats);

/**
 * Statistically analyzes of all the simulations run by the given simulator and returns the results.
 * @param simulator The simulator who's simulations should be statistically analyzed.
 * @return The results of the statistical analysis.
 */
stats_t stats_analyze(const simulator_t* simulator);

/**
 * Prints the given statistics.
 * @param stats The statistics that should be printed.
 */
void stats_print(const stats_t* stats);
