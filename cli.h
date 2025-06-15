#pragma once

#include "distribution.h"
#include "snakesandladders.h"

#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define OPTVAL_CONFIGFILE_DEFAULT 0                 // The default configuration file
#define OPTVAL_WIDTH_DEFAULT 10ul                   // The default width of the playing field
#define OPTVAL_WIDTH_MIN 2ul                        // The minimum width of the playing field
#define OPTVAL_WIDTH_MAX ULONG_MAX                  // The maximum width of the playing field
#define OPTVAL_HEIGHT_DEFAULT 10ul                  // The default height of the playing field
#define OPTVAL_HEIGHT_MIN 2ul                       // The minimum height of the playing field
#define OPTVAL_HEIGHT_MAX ULONG_MAX                 // The maximum height of the playing field
#define OPTVAL_DIE_SIDES_DEFAULT 6ul                // The default number of sides the used die has
#define OPTVAL_DIE_SIDES_MIN 1ul                    // The minimum number of sides the used die has
#define OPTVAL_DIE_SIDES_MAX ULONG_MAX              // The maximum number of sides the used die has
#define OPTVAL_EXACT_ENDING_DEFAULT false           // The default activation of the exact ending
#define OPTVAL_DISTRIBUTION_DEFAULT DISTR_UNIFORM   // The default distribution of randomly generated values

/**
 * Flags for every cli argument setting.
 */
typedef enum cli_args_flag_t {
    CLIAFLAG_NONE             = 0,
    CLIAFLAG_HELP             = 1 << 0,
    CLIAFLAG_CONFIGFILE       = 1 << 1,
    CLIAFLAG_WIDTH            = 1 << 2,
    CLIAFLAG_HEIGHT           = 1 << 3,
    CLIAFLAG_DIE_SIDES        = 1 << 4,
    CLIAFLAG_EXACT_ENDING     = 1 << 5,
    CLIAFLAG_DISTRIBUTION     = 1 << 6,
    CLIAFLAG_SNAKESANDLADDERS = 1 << 7,
} cli_args_flag_t;

/**
 * Type used to store multiple cli_args_flag_ts (bitwise ORed)
 */
typedef uint8_t cli_args_flags_t; 

/**
 * Command line interface arguments.
 * Can be parsed via the cli_parse function. 
 */
typedef struct cli_args_t {
    cli_args_flags_t setargsflags;          // Flags that indicate which values where set by the user (bitwise ORed cli_args_flag_ts)
    char* configfile;                       // The filepath of the configuration file
    size_t width;                           // The width of the playing field
    size_t height;                          // The height of the playing field
    size_t die_sides;                       // The number of sides the used die has
    bool exact_ending;                      // Enables/Disables the exact ending. The player must land exactly in the ending cell to end the game.
    distribution_t distribution;            // The distribution of randomly generated values
    snakesandladders_t snakesandladders;    // The snakes and ladders specified in the cli arguments and/or in the config file
} cli_args_t;

/**
 * Configuration file content stored as contiguous memory block in similar format as the command line arguments are stored/referenced.
 */
typedef struct cli_configfile_args_t {
    int argc;               // The number of elements in argv
    char** argv;            // The array containing the config file contents split into arguments.
} cli_configfile_args_t;

/**
 * Struct for a file position.
 */
typedef struct filepos_t {
    const char* filepath;       // The path to the file
    size_t row;                 // The row of the current position
    size_t col;                 // The column of the current position
} filepos_t;

/**
 * Struct to store the state of getopt.
 */
typedef struct getopt_state_t {
    int optind;
    char* optarg;
    int optopt;
} getopt_state_t;

/**
 * Creates a copy of the current getopt state.
 * @return The current getopt state.
 */
getopt_state_t getopt_state();

/**
 * Sets the getopt state to the given state.
 * @param state The state that getopt should be set to. If not given no action is performed.
 */
void getopt_state_set(const getopt_state_t* state);

/**
 * Frees the contents of the given cli_args freeing it's snakes and ladders and clearing all values.
 * @param cli_args The cli_args whose content should be freed.
 */
void cli_args_free(cli_args_t* cli_args);

/**
 * Parses the given comand-line arguments by forwarding argc and argv to the cli_parse_args function.
 * It's parameter initoptind is set to 1 and isconfigfile is set to false.
 * @param argc The argument count.
 * @param argv The argument values that should be parsed.
 * @return The parsed arguments.
 */
cli_args_t cli_parse(int argc, char* argv[]);

/**
 * Parses the given comand-line arguments
 * @param argc The argument count.
 * @param argv The argument values that should be parsed.
 * @param intioptind The index of the first argument in the argv array that should be parsed.
 * The default is 1 which skips the first argument (because this is commonly the program path).
 * @param isconfigfile Indicates whether these arguments came from a config file. If so the -c, --config-file option is disabled.
 * @return The parsed arguments.
 */
cli_args_t cli_parse_args(int argc, char* argv[], int initoptind, bool isconfigfile);

/**
 * Parses the options of the given comand-line arguments by using the getopt_long function.
 * If no cli_args is given no action is performed.
 * @param cli_args The cli arguments the read options should be stored in.
 * @param argc The argument count.
 * @param argv The argument values that should be parsed.
 * @param initoptind The value optind is set to before parsing the arguments. This is the index of the first argument to
 * parse in the argv array. The default is 1 which skips the first argument (because this is commonly the program path).
 * @param optstring The option string given to the getopt_long function as parameter optstring.
 * @param longopt The long options given to the getopt_long function as parameter longopts.
 * @returns The given cli_args in which the read options are stored, 0 if no cli_args was given.
 */
cli_args_t* cli_parse_opts(cli_args_t* cli_args, int argc, char* argv[], int initoptind, const char* optstring, const struct option* longopts);

/**
 * Prints the cli arguments.
 */
void cli_args_print(cli_args_t* cli_args);

/**
 * Prints the command line interface help text about the pfusch interpreter.
 */
void cli_help();

/**
 * Reads the file at the given filepath and stores it's content in cli_configfile_args_t format.
 * If no filepath was given no action is performed.
 * If the file could not be read an appropriate error message is output on stderr and the program terminates with exit code 1.
 * @param filepath The path of the file that should be read.
 * @return The read configfile arguments. 
 */
cli_configfile_args_t cli_read_configfile(const char* filepath);

/**
 * Reads the next argument from the file. The reading process starts at the current position in the file.
 * The file content is split similarly to how a linux command prompt like bash splits input.
 * First, all whitespace characters (checked with the iswhitespace function) are skipped.
 * The value is read until another whitespace character is found.
 * If the value is quoted meaning it contains a quotation mark character " or ' the quoted section ends at the
 * next found equal quotation mark. After a quoted section the value continues as normal
 * until the next whitespace character is found. A value can contain multiple quoted sections.
 * The beginning and ending quotation marks of quoted sections of a value are not included in the value.
 * Outside quoted sections characters can be escaped with the escape character (backslash \).
 * This allows for values that contain whitespace characters and/or quotation marks as part of the value.
 * @param file The file to read the argument from.
 * @param argsstr (optional) The string the read argument is appended to. The array must have elementsize == 1 or 0 (defaults to 1).
 * If not given a new string is allocated and ownership is transferred to the user.
 * @param pos (optional) The file position. If it is given it is used to error messaging when an error occurs.
 * @param error (optional) The address the error code should be stored at. If not given the error code is not stored.
 * - 0 successfully read argument.
 * - 1 unable to add character to the argument
 * - 2 argument ended in escape mode
 * - 3 argument ended with unclosed quoted section
 * @return The read argument value, 0 if no file was given or it does not contain a value at/after the
 * current file position or if the value ended in escape mode or with an unclosed quoted section.
 * If argsstr is not given the ownership of the newly allocated string is transferred to the user. 
 */
char* cli_read_configfile_arg(FILE* file, array_t* argsstr, filepos_t* pos, int* error);

/**
 * Parses the value of the just previously read option opt to a uint64 requiring it to lie in the interval [valmin, valmax] (inclusive).
 * The parsed string is read from the global variable optarg
 * @param opt The just previously read option.
 * @param valmin The minimum allowed value.
 * @param valmax The maximum allowed value.
 * @return The parsed value.
 */
uint64_t cli_parse_opt_uint64(char opt, uint64_t valmin, uint64_t valmax);

/**
 * Reads and parses the snakes and ladders in the given argv array of size argc beginning at index 0.
 * All parsed snakes and ladders are added to the args->snakesandladders array.
 * If args was not given the snakes and ladders are still parsed but not stored anywhere.
 * If any snake or ladder could not be parsed or added to the array an appropriate error message
 * is output on stderr and the program is terminated with exit code 1.
 * @param args The cli arguments the parsed snakes and ladders should be added to.
 * @param argc The element count of the argv array.
 * @param argv The array containing all strings that should be parsed to snakes and ladders.
 */
void cli_read_sals(cli_args_t* args, int argc, char* argv[]);
