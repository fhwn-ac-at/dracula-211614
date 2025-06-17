#pragma once

#include "graph.h"

#define GAME_WIDTH_MIN 2lu
#define GAME_HEIGHT_MIN 2lu

// forward declarations
typedef struct cli_args_t cli_args_t;

typedef struct game_t {
    size_t width;           // The width of the playing field
    size_t height;          // The height of the playing field
    adjmat_t adjmat;        // The graph representing and connecting the cells of the playing field
} game_t;

/**
 * Sets up a snakes and ladders game derived from the given cli arguments.
 * If the cli_args describe an invalid game an appropriate error message
 * is output on stderr and the program is terminated with exit code 1.
 * @param cli_args The cli arguments the game should be derived from.
 * @return The setup game.
 */
game_t game_setup(cli_args_t* cli_args);

/**
 * Prints the playing field of the game.
 * @param game The game that should be printed.
 */
void game_print(game_t* game);
