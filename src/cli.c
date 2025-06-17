#include "cli.h"

#include "assetmanager.h"
#include "cvts.h"
#include "macros.h"
#include "numbers.h"
#include "str.h"

#include <stdlib.h>

getopt_state_t getopt_state() {
    return (getopt_state_t){ optind, optarg, optopt };
}

void getopt_state_set(const getopt_state_t* state) {
    if (!state)
        return;
    optind = state->optind;
    optarg = state->optarg;
    optopt = state->optopt;
    #ifndef __GLIBC__
    optreset = 1;
    #endif
}

filepos_t filepos_create(const char* filepath) {
    return (filepos_t){ filepath, 1, 1 };
}

void filepos_advance(filepos_t* filepos, char character) {
    if (!filepos)
        return;
    if (character == '\n') {
        filepos->row++;
        filepos->col = 1;
    } else {
        filepos->col++;
    }
}

void cli_args_free(cli_args_t *cli_args) {
    if (!cli_args)
        return;
    distr_free(&cli_args->distribution);
    sals_free(&cli_args->snakesandladders);
    *cli_args = (cli_args_t){};
}

cli_args_t cli_parse(int argc, char* argv[]) {
    return cli_parse_args(argc, argv, 1, false);
}

cli_args_t cli_parse_args(int argc, char* argv[], int initoptind, bool isconfigfile) {
    cli_args_t args = {
        .setargsflags = CLIAFLAG_NONE,
        .configfile = OPTVAL_CONFIGFILE_DEFAULT,
        .width = OPTVAL_WIDTH_DEFAULT,
        .height = OPTVAL_HEIGHT_DEFAULT,
        .die_sides = OPTVAL_DIE_SIDES_DEFAULT,
        .exact_ending = OPTVAL_EXACT_ENDING_DEFAULT,
        .distribution = OPTVAL_DISTRIBUTION_DEFAULT,
        .snakesandladders = sals_create(0)
    };
    assetmanager_add(&args, (deallocator_fn_t)cli_args_free);

    // define options
    const char* optstring;
    struct option longopts[8];
    if (isconfigfile) {
        // disable option -c --config-file if args came from a config file
        optstring = ":h:x:y:s:ed:";
        longopts[0] = (struct option){ "help"        , 0, 0, 'h' };
        longopts[1] = (struct option){ "width"       , 1, 0, 'x' };
        longopts[2] = (struct option){ "height"      , 1, 0, 'y' };
        longopts[3] = (struct option){ "die-sides"   , 1, 0, 's' };
        longopts[4] = (struct option){ "exact-ending", 0, 0, 'e' };
        longopts[5] = (struct option){ "distribution", 1, 0, 'd' };
        longopts[6] = (struct option){ 0             , 0, 0, 0   };
        longopts[7] = (struct option){ 0             , 0, 0, 0   };
    } else {
        optstring = ":hc:x:y:s:ed:";
        longopts[0] = (struct option){ "help"        , 0, 0, 'h' };
        longopts[1] = (struct option){ "config-file" , 1, 0, 'c' };
        longopts[2] = (struct option){ "width"       , 1, 0, 'x' };
        longopts[3] = (struct option){ "height"      , 1, 0, 'y' };
        longopts[4] = (struct option){ "die-sides"   , 1, 0, 's' };
        longopts[5] = (struct option){ "exact-ending", 0, 0, 'e' };
        longopts[6] = (struct option){ "distribution", 1, 0, 'd' };
        longopts[7] = (struct option){ 0             , 0, 0, 0   };
    }
    // parse options
    cli_parse_opts(&args, argc, argv, initoptind, optstring, longopts);

    if (!isconfigfile) {
        // validate and build distribution (build preset / validate and complete custom distribution)
        int error = distr_build(&args.distribution, args.die_sides);
        if (error) {
            fprintf(stderr, "%serror:%s invalid distribution. ", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
            switch (error) {
                case 1:
                    fprintf(stderr, "no distribution given.\n");
                    exit(1);
                case 2:
                    fprintf(stderr, "unknown preset.\n");
                    exit(1);
                case 3:
                    fprintf(stderr, "more weights than die sides.\n");
                    exit(1);
                case 4:
                    fprintf(stderr, "unable to add weight.\n");
                    exit(1);
                case 5:
                    fprintf(stderr, "die sides must be even but is %lu.\n", args.die_sides);
                    exit(1);
                case 6:
                    fprintf(stderr, "die sides must not be 0.\n");
                    exit(1);
                default:
                    fprintf(stderr, "unkown error\n");
                    exit(1);
            }
        }
        // check if sum of distribution weights is 0
        bool iszero = true;
        for (size_t i = 0; iszero && i < args.distribution.weights.size; i++)
            if (*(size_t*)array_get(&args.distribution.weights, i) != 0)
                iszero = false;
        if (iszero) {
            fprintf(stderr, "%serror:%s invalid distribution. weight sum is 0.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
            exit(1);
        }
    }

    // read snakes and ladders given as arguments
    cli_read_sals(&args, argc - optind, argv + optind);
    if (optind != argc)
        optind = argc;

    return args;
}

cli_args_t* cli_parse_opts(cli_args_t* cli_args, int argc, char* argv[], int initoptind, const char* optstring, const struct option* longopts) {
    if (!cli_args)
        return 0;

    // reset getopt state
    getopt_state_set(&(getopt_state_t){ initoptind, 0, 0 });
    
    // parse options
    int opt = -1;
    while ((opt = getopt_long(argc, argv, optstring, longopts, 0)) != -1) {
        switch (opt) {
            case 'h':
            {
                cli_args->setargsflags |= CLIAFLAG_HELP;
                cli_help();
                exit(0);
                break;
            }
            case 'c':
            {
                cli_args->setargsflags |= CLIAFLAG_CONFIGFILE;

                // store previous getopt state
                getopt_state_t getoptstate = getopt_state();

                // read config file as arguments
                cli_configfile_args_t config_args = cli_read_configfile(optarg);

                #ifdef DEBUG
                printf("config_args[%d] { ", config_args.argc);
                for (int i = 0; i < config_args.argc; i++)
                    printf("\"%s\"%s ", config_args.argv[i], i != config_args.argc - 1 ? "," : "");
                printf("}\n");
                #endif

                // parse config file arguments
                cli_args_t config_cli_args = cli_parse_args(config_args.argc, config_args.argv, 0, true);
                #ifdef DEBUG
                printf("config_");
                cli_args_print(&config_cli_args);
                #endif

                // transfer config file cli arguments into cli arguments (only argument settings that where set in config file)
                if (config_cli_args.setargsflags & CLIAFLAG_WIDTH)
                    cli_args->width = config_cli_args.width;
                if (config_cli_args.setargsflags & CLIAFLAG_HEIGHT)
                    cli_args->height = config_cli_args.height;
                if (config_cli_args.setargsflags & CLIAFLAG_DIE_SIDES)
                    cli_args->die_sides = config_cli_args.die_sides;
                if (config_cli_args.setargsflags & CLIAFLAG_EXACT_ENDING)
                    cli_args->exact_ending = config_cli_args.exact_ending;
                if (config_cli_args.setargsflags & CLIAFLAG_DISTRIBUTION) {
                    distr_free(&cli_args->distribution);
                    cli_args->distribution = config_cli_args.distribution;
                    config_cli_args.distribution = distr_create_empty();
                }
                if (config_cli_args.setargsflags & CLIAFLAG_SNAKESANDLADDERS) {
                    if (cli_args->snakesandladders.array.size == 0) {
                        cli_args->snakesandladders = config_cli_args.snakesandladders;
                        config_cli_args.snakesandladders = sals_create(0);
                    } else {
                        for (size_t i = 0; i < config_cli_args.snakesandladders.array.size; i++)
                            sals_add(&cli_args->snakesandladders, sals_get(&config_cli_args.snakesandladders, i));
                    }
                }

                // free resources
                assetmanager_free(&config_cli_args);
                if (config_args.argc != 0) {
                    free(config_args.argv[0]);
                    free(config_args.argv);
                }
                
                // restore previous getopt state
                getopt_state_set(&getoptstate);
                break;
            }
            case 'x':
            {
                cli_args->setargsflags |= CLIAFLAG_WIDTH;
                cli_args->width = cli_parse_opt_uint64(opt, OPTVAL_WIDTH_MIN, OPTVAL_WIDTH_MAX);
                break;
            }
            case 'y':
            {
                cli_args->setargsflags |= CLIAFLAG_HEIGHT;
                cli_args->height = cli_parse_opt_uint64(opt, OPTVAL_HEIGHT_MIN, OPTVAL_HEIGHT_MAX);
                break;
            }
            case 's':
            {
                cli_args->setargsflags |= CLIAFLAG_DIE_SIDES;
                cli_args->die_sides = cli_parse_opt_uint64(opt, OPTVAL_DIE_SIDES_MIN, OPTVAL_DIE_SIDES_MAX);
                break;
            }
            case 'e':
            {
                cli_args->setargsflags |= CLIAFLAG_EXACT_ENDING;
                cli_args->exact_ending = true;
                break;
            }
            case 'd':
            {
                cli_args->setargsflags |= CLIAFLAG_DISTRIBUTION;
                int error = 0;
                distribution_t distr = strtodistr(optarg, &error);
                if (error) {
                    fprintf(stderr, "%serror:%s invalid distribution '%s'. ", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), optarg);
                    switch (error) {
                        case 0:
                            break;
                        case 1:
                            fprintf(stderr, "no string given.\n");
                            exit(1);
                        case 2:
                            fprintf(stderr, "unable to duplicate string.\n");
                            exit(1);
                        case 3:
                            fprintf(stderr, "weight value out of range (overflow/underflow).\n");
                            exit(1);
                        case 4:
                            fprintf(stderr, "invalid characters in weight (not a number).\n");
                            exit(1);
                        case 5:
                            fprintf(stderr, "remaining characters after number in weight string.\n");
                            exit(1);
                        default:
                            fprintf(stderr, "unknown error.\n");
                            exit(1);
                    }
                }
                cli_args->distribution = distr;
                break;
            }
            case ':':
            {
                fprintf(stderr, "%serror:%s missing value for option '%c'.\n", FMT(FMTVAL_FG_RED), FMT(FMTVAL_FG_DEFAULT), optopt);
                exit(1);
            }
            case '?':
            {
                fprintf(stderr, "%serror:%s unknown option '%c'.\n", FMT(FMTVAL_FG_RED), FMT(FMTVAL_FG_DEFAULT), optopt);
                exit(1);
            }
        }
    }

    return cli_args;
}

void cli_args_print(cli_args_t* cli_args) {
    if (!cli_args) {
        printf("cli_args = %s\n", (char*)0);
        return;
    }
    printf(
        "cli_args = {\n"
        "  setargsflags     = 0b%u%u%u%u%u%u%u%u\n"
        "  configfile       = %s%s%s\n"
        "  width            = %lu\n"
        "  height           = %lu\n"
        "  die_sides        = %lu\n"
        "  exact_ending     = %s\n"
        "  distribution     = {\n"
        "    preset  = %s (%d)\n"
        "    weights = [%lu] {",
        (bool)(cli_args->setargsflags & CLIAFLAG_SNAKESANDLADDERS),
        (bool)(cli_args->setargsflags & CLIAFLAG_DISTRIBUTION),
        (bool)(cli_args->setargsflags & CLIAFLAG_EXACT_ENDING),
        (bool)(cli_args->setargsflags & CLIAFLAG_DIE_SIDES),
        (bool)(cli_args->setargsflags & CLIAFLAG_HEIGHT),
        (bool)(cli_args->setargsflags & CLIAFLAG_WIDTH),
        (bool)(cli_args->setargsflags & CLIAFLAG_CONFIGFILE),
        (bool)(cli_args->setargsflags & CLIAFLAG_HELP),
        cli_args->configfile ? "\"" : "", cli_args->configfile, cli_args->configfile ? "\"" : "",
        cli_args->width,
        cli_args->height,
        cli_args->die_sides,
        cli_args->exact_ending ? "true" : "false",
        distr_preset_infos[cli_args->distribution.preset].name, cli_args->distribution.preset,
        cli_args->distribution.weights.size
    );
    if (cli_args->distribution.weights.size != 0) {
        printf(" ");
        for (size_t i = 0; i < cli_args->distribution.weights.size; i++) {
            const size_t* weight = array_get(&cli_args->distribution.weights, i);
            printf("%lu%s ", *weight, i != cli_args->distribution.weights.size - 1 ? "," : "");
        }
    }
    printf("}\n  },\n");
    printf("  snakesandladders = [%lu] {", cli_args->snakesandladders.array.size);
    if (cli_args->snakesandladders.array.size != 0) {
        printf("\n");
        for (size_t i = 0; i < cli_args->snakesandladders.array.size; i++) {
            const snakeorladder_t* sol = sals_get(&cli_args->snakesandladders, i);
            printf("    %lu-%lu%s\n", sol->src, sol->dst, i != cli_args->snakesandladders.array.size - 1 ? "," : "");
        }
        printf("  ");
    }
    printf("}\n}\n");
}

void cli_help() {
    printf(
        "sals " VERSION "\n"
        "Usage: sals [options] <snake-or-ladder>...\n"
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
        "  -c, --config-file %sval%s     The filepath of the configuration file which is processed the same way as the command line arguments\n"
        "                             are processed after splitting the file content similarly as e.g. the linux bash splits it's input.\n"
        "                             The option -c, --config-file is not allowed inside a configuration file.\n"
        "                             Non-option argument values that appear in the cli arguments before the -c, --config-file option are lost\n"
        "                             because the necessary argument reordering fails after processing the configuration file.\n"
        "  -x, --width %sval%s           The width of the playing field which must be an integer value >= %lu. The default value is %lu.\n"
        "  -y, --height %sval%s          The height of the playing field which must be an integer value >= %lu. The default value is %lu.\n"
        "  -s, --die-sides %sval%s       The number of sides the used die has which must be an integer value >= %lu. The default value is %lu.\n"
        "  -e, --exact-ending        Enables the exact ending requirement. To end a game the player must land exactly on the last cell\n"
        "                             of the playing field. If a rolled die would overshoot the last cell the player is not moved.\n"
        "  -d, --distribution %sval%s    The distribution when generating a random number in a die. The value can be a sequence of weights\n"
        "                             separated by commas in the format '%sW1%s,%sW2%s,%sW3%s,%sW4%s,...,%sWn%s' where %sWn%s is the weight of the %sn%s-th die side.\n"
        "                             Thus this specified weight sequence must have a weight count %sn%s less than or equal the die sides %ss%s (%sn%s <= %ss%s).\n"
        "                             If less weights are specified than the number of die sides (%sn%s < %ss%s) the remaining die sides are automatically weighted with 0.\n"
        "                             Each weight %sWi%s is an unsigned integer value. The distribution describes that the side %si%s's probability to be diced\n"
        "                             in a dice roll %sPi%s is %sWi%s divided by the sum of all %sW%ss %sS%s.\n"
        "                             i.e. %ss%s  %s// number of die sides (unsigned integer)%s\n"
        "                                  %sn%s  %s// number of die specified weights in the weight sequence (unsigned integer)%s\n"
        "                                  %si%s  %s// an arbitrary integer from 1 to %ss%s inclusive%s\n"
        "                                  %sWi%s  %s// the weight of the side %si%s (unsigned integer)%s\n"
        "                                  %sS%s = %sW1%s + %sW2%s + %sW3%s + %sW4%s + ... + %sWs%s  %s// sum of the weights of all sides 1 through %ss%s inclusive%s\n"
        "                                  %sPi%s = %sWi%s / %sS%s  %s// probability of side %si%s to be diced in a dice roll%s\n"
        "                             Alternatively to providing a weight sequence as described above, it is possible to use one of the following presets\n"
        "                             whose weights are calculated in a certain way based on the die sides %ss%s.\n"
        "                             - uniform    A uniform distribution %s(default)%s\n"
        "                                           e.g. for %ss%s = 6 the weights are 1,1,1,1,1,1\n"
        "                             - twodice    The distribution that arises when using two uniform and equal dice with %ss%s/2 sides each and using the\n"
        "                                           sum of the two diced values as overall dice result.\n"
        "                                           e.g. for %ss%s = 12 (two uniform dice with %ss%s/2 = 6 sides each)\n"
        "                                                SUM\x1b(0x\x1b(B  1  2  3  4  5  6 \n"
        "                                                \x1b(0qqqnqqqqqqqqqqqqqqqqqqq\x1b(B\n"
        "                                                 1 \x1b(0x\x1b(B  2  3  4  5  6  7 \n"
        "                                                 2 \x1b(0x\x1b(B  3  4  5  6  7  8 \n"
        "                                                 3 \x1b(0x\x1b(B  4  5  6  7  8  9 \n"
        "                                                 4 \x1b(0x\x1b(B  5  6  7  8  9 10 \n"
        "                                                 5 \x1b(0x\x1b(B  6  7  8  9 10 11 \n"
        "                                                 6 \x1b(0x\x1b(B  7  8  9 10 11 12 \n"
        "                                                %s// count how often side (sum) is hit to determine their weights.%s\n"
        "                                                %s// these are all possible ways to dice each value (sum) when using two uniform 6-sided dice%s\n"
        "                                                -> distribution weights: 0,1,2,3,4,5,6,5,4,3,2,1\n"
        "\n",
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), OPTVAL_WIDTH_MIN, OPTVAL_WIDTH_DEFAULT,
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), OPTVAL_HEIGHT_MIN, OPTVAL_HEIGHT_DEFAULT,
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT), OPTVAL_DIE_SIDES_MIN, OPTVAL_DIE_SIDES_DEFAULT,
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE), FMT(FMTVAL_UNDERLINE), FMT(FMTVAL_NO_UNDERLINE),
        FMT(FMTVAL_FG_BRIGHT_BLACK), FMT(FMTVAL_FG_DEFAULT),
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

    filepos_t pos = filepos_create(filepath);
    cli_configfile_args_t config_args = {};
    array_t args = array_create(0, sizeof(char*), 0);
    array_t argsstr = array_create(0, sizeof(char), 0);
    const char* arg;
    size_t argpos;
    bool startedarg = false;
    int error = 0;

    // add config filepath as argv[0] element (skipped during parsing in getopt_long function)
    for (const char* filec = filepath; *filec; filec++) {
        arg = array_add(&argsstr, filec);
        if (!arg) {
            error = 1;
            fprintf(stderr, "%serror:%s unable to add filepath argument to args string at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos.filepath, pos.row, pos.col);
            break;
        }
        if (!startedarg) {
            argpos = arg - (char*)argsstr.data;
            startedarg = true;
        }
    }
    if (!error) {
        // add null-terminator to argument
        char terminator = '\0';
        if (!array_add(&argsstr, &terminator)) {
            error = 4;
            fprintf(stderr, "%serror:%s unable to add null-terminator to filepath argument at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos.filepath, pos.row, pos.col);
        } else {
            // add index of filepath to args array
            config_args.argc++;
            size_t* argref = array_add(&args, &argpos);
            if (!argref) {
                error = 2;
                fprintf(stderr, "%serror:%s unable to add filepath argument reference to args at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos.filepath, pos.row, pos.col);
            } else {
                // read arguments, add them to args string and store their index (where they start in argsstr) in args array
                while ((arg = cli_read_configfile_arg(file, &argsstr, &pos, &error))) {
                    config_args.argc++;
                    argpos = arg - (char*)argsstr.data;
                    argref = array_add(&args, &argpos);
                    if (!argref) {
                        error = 2;
                        fprintf(stderr, "%serror:%s unable to add argument reference to args at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos.filepath, pos.row, pos.col);
                        break;
                    }
                }
                for (size_t i = 0; i < args.size; i++)
                    ((char**)args.data)[i] += (size_t)argsstr.data;
                config_args.argv = args.data;
            }
        }
    }

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
    while ((c = fgetc(file)) != EOF && iswhitespace(c))
        filepos_advance(pos, c);
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
        if (mode == esc) {
            mode = 0;
        } else if (mode == qu1 || mode == qu2) {
            if (c == mode) {
                mode = 0;
                filepos_advance(pos, c);
                continue;
            }
        } else if (c == qu1 || c == qu2) {
            mode = c;
            filepos_advance(pos, c);
            continue;
        } else if (c == esc) {
            mode = esc;
            filepos_advance(pos, c);
            continue;
        } else if (iswhitespace(c)) {
            filepos_advance(pos, c);
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
        filepos_advance(pos, c);
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

    // add null-terminator to argument
    char terminator = '\0';
    char* termaddr = array_add(arg, &terminator);
    if (!termaddr) {
        if (error)
            *error = 4;
        if (pos)
            fprintf(stderr, "%serror:%s unable to add null-terminator to argument at %s:%lu:%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), pos->filepath, pos->row, pos->col);
        else
            fprintf(stderr, "%serror:%s unable to add null-terminator to argument in configfile.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        if (!argsstr)
            array_free(arg);
        return 0;
    }
    if (!startedarg)
        argstart = termaddr - (char*)arg->data;

    if (error)
        *error = 0;
    return &((char*)arg->data)[argstart];
}

uint64_t cli_parse_opt_uint64(char opt, uint64_t valmin, uint64_t valmax) {
    size_t value = 0;
    int error = strtouint64(optarg, &value);
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
    if (args && argc != 0)
        args->setargsflags |= CLIAFLAG_SNAKESANDLADDERS;
    for (int i = 0; i < argc; i++) {
        int error = 0;
        snakeorladder_t sol = strtosol(argv[i], &error);
        if (error) {
            fprintf(stderr, "%serror:%s invalid snake-or-ladder %sa%s-%sb%s '%s'. ", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT),
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
