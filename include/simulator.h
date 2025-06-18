#pragma once

#include "game.h"
#include "snakesandladders.h"

#include <threads.h>

#define SIMULATION_DICE_LIMIT 10000lu       // The maximum number of times a game simulation is allowed to dice before aborting execution. (The simulation potentially ran into an infinite loop)

/**
 * Struct for an optional size_t. Can optionally have value of type size_t.
 */
typedef struct optional_size_t {
    bool present;                   // Indicates if this optional has a value
    size_t value;                   // The stored value
} optional_size_t;

/**
 * Struct used for managing simulations for a specific game.
 */
typedef struct simulator_t {
    const game_t* game;             // The game simulations should be run on
    array_t sals;                   // The snakes and ladders of the game
    array_t solidxs;                // Maps each cell index to the index of it's corresponding snake or ladder in soldsts array
    array_t sims;                   // The array of simulations
} simulator_t;

/**
 * Struct for a simulation of a snakes and ladders game.
 */
typedef struct simulation_t {
    simulator_t* simulator;         // The simulator the simulation belongs to
    thrd_t thread;                  // The identifier of the thread the simulation is run on.
    bool aborted;                   // Indicates if the simulation was aborted because the SIMULATION_DICE_LIMIT was reached but the game is still running (potentially ran into an infinite loop)
    size_t dices;                   // The number of times the die was diced during the simulation
    array_t soluses;                // The number of times each snake or ladder was used during the simulation
} simulation_t;

/**
 * Creates an empty simulator.
 * @return The created empty simulator.
 */
simulator_t simulator_create_empty();

/**
 * Creates a new simulator for the given game with the given simulation count.
 * @param game The game that should be simulated by simulations managed by the created simulator.
 * @param simcount The number of simulations that should be run on the specified game.
 * @return The created simulator, an empty simulator if no game was given or simcount is 0.
 */
simulator_t simulator_create(const game_t* game, size_t simcount);

/**
 * Frees the given simulator freeing it's soldsts and solidxs arrays and resets to an empty simulator.
 * @param simulator The simulator that should be reset.
 */
void simulator_free(simulator_t* simulator);

/**
 * Simulates the given game the specified number of times.
 * The simulations are run simultaneously on separate threads.
 * @param game The game that should be simulated.
 * @param simcount The number of simulations that should be run.
 */
void simulate(const game_t* game, size_t simcount);

/**
 * Runs the given simulation. The simulation holds a reference to the simulator the
 * simulation belongs to which contains information about the game that should be simulated.
 * @param simulation The simulation that should be run.
 * @return 0 if the simulation ran successfully, the error code of an occurred error otherwise.
 * - 1 no simulation given
 */
int simulation_run(simulation_t* simulation);

/**
 * Prints the given simulator.
 * @param simulator The simulator that should be printed.
 */
void simulator_print(const simulator_t* simulator);
