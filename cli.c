#include "cli.h"

#include "cvts.h"
#include "macros.h"
#include "parsenum.h"
#include "snakesandladders.h"
#include "str.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

cli_args_t cli_parse(int argc, char* argv[]) {
    cli_args_t args = {
        .configfile = OPTVAL_CONFIGFILE_DEFAULT,
        .width = OPTVAL_WIDTH_DEFAULT,
        .height = OPTVAL_HEIGHT_DEFAULT,
        .die_sides = OPTVAL_DIE_SIDES_DEFAULT,
        .exact_ending = OPTVAL_EXACT_ENDING_DEFAULT,
        .distribution = OPTVAL_DISTRIBUTION_DEFAULT
    };

    const char* shortopts = "hc:x:y:s:ed:";
    struct option longopts[7] = {
        { "help"        , 0, 0, 'h' },
        { "config-file" , 1, 0, 'c' },
        { "width"       , 1, 0, 'x' },
        { "height"      , 1, 0, 'y' },
        { "die-sides"   , 1, 0, 's' },
        { "exact-ending", 0, 0, 'e' },
        { "distribution", 1, 0, 'd' }
    };

    int opt = -1;
    while ((opt = getopt_long(argc, argv, shortopts, longopts, 0)) != -1) {
        switch (opt) {
            case 'h':
            {
                cli_help();
                exit(0);
                break;
            }
            case 'c':
            {
                // TODO read config file
                printf("-%c // TODO //\n", opt);
            }
            case 'x':
            {
                size_t width = 0;
                int error = parse_uint64(optarg, &width);
                switch (error) {
                    case 1:
                        fprintf(stderr, "%serror:%s -%c no string given.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt);
                        exit(1);
                    case 2:
                        fprintf(stderr, "%serror:%s -%c value out of range (%s).\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, optarg);
                        exit(1);
                    case 3:
                    case 4:
                        fprintf(stderr, "%serror:%s -%c not a number '%s'.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, optarg);
                        exit(1);
                    default:
                        break;
                }
                if (width < OPTVAL_WIDTH_MIN) {
                    fprintf(stderr, "%serror:%s -%c %lu less than minimum %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, width, OPTVAL_WIDTH_MIN);
                    exit(1);
                }
                args.width = width;
                break;
            }
            case 'y':
            {
                size_t height = 0;
                int error = parse_uint64(optarg, &height);
                switch (error) {
                    case 1:
                        fprintf(stderr, "%serror:%s -%c no string given.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt);
                        exit(1);
                    case 2:
                        fprintf(stderr, "%serror:%s -%c value out of range (%s).\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, optarg);
                        exit(1);
                    case 3:
                    case 4:
                        fprintf(stderr, "%serror:%s -%c not a number '%s'.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, optarg);
                        exit(1);
                    default:
                        break;
                }
                if (height < OPTVAL_HEIGHT_MIN) {
                    fprintf(stderr, "%serror:%s -%c %lu less than minimum %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, height, OPTVAL_HEIGHT_MIN);
                    exit(1);
                }
                args.height = height;
                break;
            }
            case 's':
            {
                size_t sides = 0;
                int error = parse_uint64(optarg, &sides);
                switch (error) {
                    case 1:
                        fprintf(stderr, "%serror:%s -%c no string given.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt);
                        exit(1);
                    case 2:
                        fprintf(stderr, "%serror:%s -%c value out of range (%s).\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, optarg);
                        exit(1);
                    case 3:
                    case 4:
                        fprintf(stderr, "%serror:%s -%c not a number '%s'.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, optarg);
                        exit(1);
                    default:
                        break;
                }
                if (sides < OPTVAL_DIE_SIDES_MIN) {
                    fprintf(stderr, "%serror:%s -%c %lu less than minimum %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, sides, OPTVAL_DIE_SIDES_MIN);
                    exit(1);
                }
                args.height = sides;
                break;
            }
            case '?':
            {
                fprintf(stderr, "%serror:%s unknown option '%c'\n", FMT(FMTVAL_FG_RED), FMT(FMTVAL_FG_DEFAULT), optopt);
                exit(1);
            }
        }
    }
    // process snakes and ladders
    for (; optind < argc; optind++) {
        int error = 0;
        snakeorladder_t sol = strtosol(argv[optind], &error);
        if (error) {
            fprintf(stderr, "%serror%s invalid snake-or-ladder %sa%s-%sb%s '%s'.", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT),
                FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), argv[optind]);
            switch (error) {
                case 1:
                    fprintf(stderr, "no string given.\n");
                    exit(1);
                case 2:
                    fprintf(stderr, "unable to duplicate the given string.\n");
                    exit(1);
                case 3:
                    fprintf(stderr, "string does not contain the split character '-'.\n");
                    exit(1);
                case 4:
                    fprintf(stderr, "value %sa%s out of range (overflow/underflow).\n", FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE));
                    exit(1);
                case 5:
                    fprintf(stderr, "invalid characters in %sa%s (not a number).\n", FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE));
                    exit(1);
                case 6:
                    fprintf(stderr, "remaining characters after number in string for %sa%s.\n", FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE));
                    exit(1);
                case 7:
                    fprintf(stderr, "value %sb%s out of range (overflow/underflow).\n", FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE));
                    exit(1);
                case 8:
                    fprintf(stderr, "invalid characters in %sb%s (not a number).\n", FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE));
                    exit(1);
                case 9:
                    fprintf(stderr, "remaining characters after number in string for %sb%s.\n", FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE));
                    exit(1);
                default:
                    fprintf(stderr, "unknown error.\n");
                    exit(1);
            }
        }
    }

    return args;
}

void cli_help() {
    printf(
        "snakesandladders " VERSION "\n"
        "Usage: ./snakesandladders [options] <snake-or-ladder>...\n"
        "\n"
        "  <snake-or-ladder>         A string containing two positive integers separated by a '-' character. Format: %sa%s-%sb%s.\n"
        "                             %sa%s is the index of the starting cell and %sb%s is the index of the ending cell.\n"
        "                             Ladders start in a cell before the ending cell (%sa%s < %sb%s) and propel the player forward from %sa%s to %sb%s.\n"
        "                             Snakes start in a cell after the ending cell (%sa%s > %sb%s) and propel the player backward from %sa%s to %sb%s.\n"
        "                             All specified snakes and ladders must not end in the last cell of the playing field and\n"
        "                             they must not start/end in the same cell as themselves or any other snake or ladder. (i.e. overlap is disallowed)\n"
        "                             e.g. 0-16 13-45 23-27 15-3 48-20 54-11\n"
        "\n"
        "options:\n"
        "  -h, --help                Prints this help text and terminates the program.\n"
        "  -c, --config-file %sval%s     %s// TODO //%s The filepath of the configuration file which is processed the same way as the\n"
        "                             command line arguments are processed after splitting the file content at whitespace characters.\n"
        "                             The option -c, --config-file is not allowed inside a configuration file.\n"
        "  -x, --width %sval%s           The width of the playing field which must be an integer value >= %lu. The default value is %lu.\n"
        "  -y, --height %sval%s          The height of the playing field which must be an integer value >= %lu. The default value is %lu.\n"
        "  -s, --die-sides %sval%s       The number of sides the used die has which must be an integer value >= %lu. The default value is %lu.\n"
        "  -e, --exact-ending        Enables the exact ending requirement. To end a game the player must land exactly on the last cell\n"
        "                             of the playing field. If a rolled die would overshoot the last cell the player is not moved.\n"
        "  -d, --distribution %sval%s    The distribution when generating a random number in a die. Allowed values are:\n"
        "                             - u, uniform: A uniform distribution (default)\n"
        "\n",
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), FMT(FMTVAL_BOLD), FMT(FMTVAL_NO_BOLD),
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), OPTVAL_WIDTH_MIN, OPTVAL_WIDTH_DEFAULT,
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), OPTVAL_HEIGHT_MIN, OPTVAL_HEIGHT_DEFAULT,
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), OPTVAL_DIE_SIDES_MIN, OPTVAL_DIE_SIDES_DEFAULT,
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT)
    );
}
