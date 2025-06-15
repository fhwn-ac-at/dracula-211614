#pragma once

#include "distribution.h"

#include <stdbool.h>
#include <stddef.h>

#define OPTVAL_CONFIGFILE_DEFAULT 0                 // The default configuration file
#define OPTVAL_WIDTH_DEFAULT 10ul                   // The default width of the playing field
#define OPTVAL_WIDTH_MIN 2ul                        // The minimum width of the playing field
#define OPTVAL_HEIGHT_DEFAULT 10ul                  // The default height of the playing field
#define OPTVAL_HEIGHT_MIN 2ul                       // The minimum height of the playing field
#define OPTVAL_DIE_SIDES_DEFAULT 6ul                // The default number of sides the used die has
#define OPTVAL_DIE_SIDES_MIN 1ul                    // The minimum number of sides the used die has
#define OPTVAL_EXACT_ENDING_DEFAULT false           // The default activation of the exact ending
#define OPTVAL_DISTRIBUTION_DEFAULT DISTR_UNIFORM   // The default distribution of randomly generated values

/**
 * Command line interface arguments.
 * Can be parsed via the cli_parse function. 
 */
typedef struct cli_args_t {
    char* configfile;               // The filepath of the configuration file
    size_t width;                   // The width of the playing field
    size_t height;                  // The height of the playing field
    size_t die_sides;               // The number of sides the used die has
    bool exact_ending;              // Enables/Disables the exact ending. The player must land exactly in the ending cell to end the game.
    distribution_t distribution;    // The distribution of randomly generated values
} cli_args_t;

/**
 * Parses the given comand-line arguments
 * @param argc The number of arguments.
 * @param argv The argument values that should be parsed.
 * @return The parsed arguments.
 */
cli_args_t cli_parse(int argc, char* argv[]);

/**
 * Prints the command line interface help text about the pfusch interpreter.
 */
void cli_help();
