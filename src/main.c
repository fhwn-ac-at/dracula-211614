#include "sals.h"

int main(int argc, char* argv[]) {
    assetmanager_init();

    #ifdef DEBUG
    printf("args[%d] { ", argc);
    for (int i = 0; i < argc; i++)
        printf("\"%s\"%s ", argv[i], i != argc - 1 ? "," : "");
    printf("}\n");
    #endif

    cli_args_t cli_args = cli_parse(argc, argv);
    #ifdef DEBUG
    cli_args_print(&cli_args);
    #endif

    game_t game = game_setup(&cli_args);

    printf("Snakes and Ladders Simulator\n\n");

    #ifdef DEBUG
    printf("game adjmat\n");
    adjmat_print(&game.adjmat);
    #endif

    #ifdef DEBUG
    printf("game\n");
    #endif
    game_print(&game);

    die_print(&game.die, cli_args.barlength);

    #ifdef DEBUG
    simulate_dices(&game.die, cli_args.iterations);
    #endif

    simulator_t simulator = simulate(&game, cli_args.iterations, cli_args.dicelimit);

    stats_t stats = stats_analyze(&simulator);
    stats_print(&stats);

    assetmanager_free_all();
}
