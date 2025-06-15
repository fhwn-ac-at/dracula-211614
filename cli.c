#include "cli.h"

#include "cvts.h"
#include "macros.h"
#include "parsenum.h"
#include "str.h"

#include <getopt.h>
#include <stdlib.h>

cli_args_t cli_parse(int argc, char* argv[], bool isconfigfile) {
    cli_args_t args = {
        .configfile = OPTVAL_CONFIGFILE_DEFAULT,
        .width = OPTVAL_WIDTH_DEFAULT,
        .height = OPTVAL_HEIGHT_DEFAULT,
        .die_sides = OPTVAL_DIE_SIDES_DEFAULT,
        .exact_ending = OPTVAL_EXACT_ENDING_DEFAULT,
        .distribution = OPTVAL_DISTRIBUTION_DEFAULT,
        .snakesandladders = sals_create(0)
    };

    // process options
    const char* shortopts = ":hc:x:y:s:ed:";
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
        if (isconfigfile && opt == 'c') {
            optopt = opt;
            opt = '?';
        }
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

                cli_configfile_args_t config_args = cli_read_configfile(optarg);

                printf("config_args[%d] { ", config_args.argc);
                for (size_t i = 0; i < config_args.argc; i++)
                    printf("\"%s\"%s ", config_args.argv[i], i != config_args.argc - 1 ? "," : "");
                printf("}\n");

                // cli_args_t config_cli_args = cli_parse(config_args.argc, config_args.argv, true);
                // printf("config_");
                // cli_args_print(&config_cli_args);

                if (config_args.argc != 0) {
                    free(config_args.argv[0]);
                    free(config_args.argv);
                }
                break;
            }
            case 'x':
            {
                args.width = cli_parse_opt_uint64(opt, OPTVAL_WIDTH_MIN, OPTVAL_WIDTH_MAX);
                break;
            }
            case 'y':
            {
                args.height = cli_parse_opt_uint64(opt, OPTVAL_HEIGHT_MIN, OPTVAL_HEIGHT_MAX);
                break;
            }
            case 's':
            {
                args.die_sides = cli_parse_opt_uint64(opt, OPTVAL_DIE_SIDES_MIN, OPTVAL_DIE_SIDES_MAX);
                break;
            }
            case ':':
            {
                fprintf(stderr, "%serror:%s missing value for option '%c'\n", FMT(FMTVAL_FG_RED), FMT(FMTVAL_FG_DEFAULT), optopt);
                exit(1);
            }
            case '?':
            {
                fprintf(stderr, "%serror:%s unknown option '%c'\n", FMT(FMTVAL_FG_RED), FMT(FMTVAL_FG_DEFAULT), optopt);
                exit(1);
            }
        }
    }

    // read snakes and ladders given as arguments
    cli_read_sals(&args, argc - optind, argv + optind);
    optind = argc;

    return args;
}

void cli_args_print(const cli_args_t* cli_args) {
    if (!cli_args) {
        printf("cli_args = %s\n", (char*)0);
        return;
    }
    printf(
        "cli_args = {\n"
        "  configfile       = %s%s%s\n"
        "  width            = %lu\n"
        "  height           = %lu\n"
        "  die_sides        = %lu\n"
        "  exact_ending     = %s\n"
        "  distribution     = %d\n"
        "  snakesandladders = [%lu] {",
        cli_args->configfile ? "\"" : "", cli_args->configfile, cli_args->configfile ? "\"" : "",
        cli_args->width,
        cli_args->height,
        cli_args->die_sides,
        cli_args->exact_ending ? "true" : "false",
        cli_args->distribution,
        cli_args->snakesandladders.array.size
    );
    if (cli_args->snakesandladders.array.size != 0) {
        printf("\n");
        for (size_t i = 0; i < cli_args->snakesandladders.array.size; i++) {
            snakeorladder_t* sol = &((snakeorladder_t*)cli_args->snakesandladders.array.data)[i * cli_args->snakesandladders.array.elementsize];
            printf("    %lu-%lu%s\n", sol->src, sol->dst, i != cli_args->snakesandladders.array.size ? "," : "");
        }
        printf("  ");
    }
    printf("}\n}\n");
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

cli_configfile_args_t cli_read_configfile(const char* filepath) {
    if (!filepath)
        return (cli_configfile_args_t){};

    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "%serror:%s unable to read config file '%s'\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), filepath);
        exit(1);
    }

    filepos_t pos = { filepath };
    cli_configfile_args_t config_args = {};
    array_t args = array_create(0, sizeof(char*), 0);
    array_t argsstr = array_create(0, sizeof(char), 0);
    const char* arg;
    int error = 0;

    while ((arg = cli_read_configfile_arg(file, &argsstr, &pos, &error))) {
        config_args.argc++;
        size_t argpos = arg - (char*)argsstr.data;
        array_add(&args, &argpos);
    }
    for (size_t i = 0; i < args.size; i++)
        ((char**)args.data)[i] += (size_t)argsstr.data;
    config_args.argv = args.data;

    fclose(file);

    if (error) {
        array_free(&args);
        array_free(&argsstr);
        exit(1);
    }

    return config_args;
}

char* cli_read_configfile_arg(FILE* file, array_t* argsstr, filepos_t* pos, int* error) {
    if (!file)
        return 0;
    
    // skip starting whitespaces
    char c;
    while ((c = fgetc(file)) != EOF && iswhitespace(c));
    if (c == EOF)
        return 0;
    
    // setup static const data for managing modes
    static const char esc = '\\';
    static const char qu1 = '\'';
    static const char qu2 = '"';
    
    // read argument potentially with escaped characters (\) and quoted sections (", ')
    array_t newarg;
    array_t* arg = argsstr ? argsstr : &newarg;
    if (!argsstr)
        newarg = array_create(0, sizeof(char), 0);
    size_t argstart = 0;
    bool startedarg = false;
    char mode = 0;
    do {
        if (startedarg) {
            if (c == '\n') {
                pos->row++;
                pos->col = 0;
            } else {
                pos->col++;
            }
        }
        if (mode == esc) {
            mode = 0;
        } else if (mode == qu1 || mode == qu2) {
            if (c == mode) {
                mode = 0;
                continue;
            }
        } else if (c == qu1 || c == qu2) {
            mode = c;
            continue;
        } else if (c == esc) {
            mode = esc;
            continue;
        } else if (iswhitespace(c)) {
            break;
        }
        char* addr = array_add(arg, &c);
        if (!addr) {
            if (error)
                *error = 1;
            if (pos)
                fprintf(stderr, "%serror:%s unable to add char to argument at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos->filepath, pos->row, pos->col);
            else
                fprintf(stderr, "%serror:%s unable to add char to argument in configfile.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
            if (!argsstr)
                array_free(arg);
            return 0;
        }
        if (!startedarg) {
            argstart = addr - (char*)arg->data;
            startedarg = true;
        }
    } while ((c = fgetc(file)) != EOF);

    // check for active escape mode or unclosed quoted section at the end of the argument
    if (mode == esc || mode == qu1 || mode == qu2) {
        if (mode == esc) {
            if (error)
                *error = 2;
            if (pos)
                fprintf(stderr, "%serror:%s argument ended in escape mode at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos->filepath, pos->row, pos->col);
            else
                fprintf(stderr, "%serror:%s argument ended in escape mode in configfile.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        } else {
            if (error)
                *error = 3;
            if (pos)
                fprintf(stderr, "%serror:%s argument ended with unclosed quoted section at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos->filepath, pos->row, pos->col);
            else
                fprintf(stderr, "%serror:%s argument ended with unclosed quoted section in configfile.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        }
        if (!argsstr)
            array_free(arg);
        return 0;
    }

    char terminator = '\0';
    array_add(arg, &terminator);
    if (error)
        *error = 0;
    return &((char*)arg->data)[argstart];
}

uint64_t cli_parse_opt_uint64(char opt, uint64_t valmin, uint64_t valmax) {
    size_t value = 0;
    int error = parse_uint64(optarg, &value);
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
    if (value < valmin) {
        fprintf(stderr, "%serror:%s -%c %lu less than minimum %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, value, valmin);
        exit(1);
    }
    if (value > valmax) {
        fprintf(stderr, "%serror:%s -%c %lu greater than maximum %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), opt, value, valmax);
        exit(1);
    }
    return value;
}

void cli_read_sals(cli_args_t* args, int argc, char* argv[]) {
    for (size_t i; i < argc; i++) {
        int error = 0;
        snakeorladder_t sol = strtosol(argv[i], &error);
        if (error) {
            fprintf(stderr, "%serror:%s invalid snake-or-ladder %sa%s-%sb%s '%s'.", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT),
                FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), argv[i]);
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
        if (args && !sals_add(&args->snakesandladders, &sol)) {
            fprintf(stderr, "%serror:%s unable to add snake-or-ladder '%s' to array [capacity: %lu].\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), argv[i], args->snakesandladders.array.capacity);
            exit(1);
        }
    }
}
