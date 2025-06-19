#pragma once

#include "game.h"
#include "snakesandladders.h"

#include <threads.h>

#define SIMULATION_DICE_LIMIT 10000lu      // The maximum number of times a game simulation is allowed to dice before aborting execution. (The simulation potentially ran into an infinite loop)

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
    array_t soldsts;                // The destinations of the snakes and ladders of the game (element type: size_t)
    array_t solidxs;                // Maps each cell index to the index of it's corresponding snake or ladder in soldsts array (element type: optional_size_t)
    array_t sims;                   // The array of simulations (element simulation_t)
} simulator_t;

/**
 * Struct for a simulation of a snakes and ladders game.
 */
typedef struct simulation_t {
    simulator_t* simulator;         // The simulator the simulation belongs to
    thrd_t thread;                  // The identifier of the thread the simulation is run on.
    bool aborted;                   // Indicates if the simulation was aborted because the SIMULATION_DICE_LIMIT was reached but the game is still running (potentially ran into an infinite loop)
    size_t playerpos;               // The player's position
    array_t soluses;                // The number of times each snake or ladder was used during the simulation (element type: size_t)
    array_t dices;                  // The sequence of diced sides during the simulation (element type: size_t)
} simulation_t;

/**
 * Creates an empty simulation.
 * @return The created empty simulation.
 */
simulation_t simulation_create_empty();

/**
 * Creates a new simulation for the given simulator.
 * The initial capacity of the simulation's soluses array is the number of snakes and ladders that exist in the simulator's game.
 * If no simulator was given an empty simulation is created.
 * @param simulator The simulator the created simulation belongs to.
 * @return The created simulation.
 */
simulation_t simulation_create(simulator_t* simulator);

/**
 * Frees the given simulation freeing it's soluses array and resetting to an empty simulation.
 * @param simulation The simulation that should be freed.
 */
void simulation_free(simulation_t* simulation);

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
 * Frees the given simulator freeing it's soldsts, solidxs and sims arrays and resetting to an empty simulator.
 * @param simulator The simulator that should be reset.
 */
void simulator_free(simulator_t* simulator);

/**
 * Simulates the given game the specified number of times.
 * The simulations are run simultaneously on separate threads.
 * @param game The game that should be simulated.
 * @param simcount The number of simulations that should be run.
 * @return The simulator that ran the simulations.
 */
simulator_t simulate(const game_t* game, size_t simcount);

/**
 * Runs the given simulation. The simulation holds a reference to the simulator the
 * simulation belongs to which contains information about the game that should be simulated.
 * @param simulation The simulation that should be run.
 * @return The error code, 0 on success.
 * 
 * - 0 successfully ran simulation
 * 
 * - 1 no simulation given
 */
int simulation_run(simulation_t* simulation);

/**
 * Prints the given simulator.
 * @param simulator The simulator that should be printed.
 * @param indent The number of space characters the output should be indented with.
 * @param indentfirst Indicates wether the first line should be indented.
 */
void simulator_print(const simulator_t* simulator, uint32_t indent, bool indentfirst);

/**
 * Prints the given simulation with the given indent.
 * if indentfirst is false the first line is not indented.
 * @param simulation The simulation that should be printed.
 * @param indent The number of space characters the output should be indented with.
 * @param indentfirst Indicates wether the first line should be indented.
 */
void simulation_print(const simulation_t* simulation, uint32_t indent, bool indentfirst);
