#include "assetmanager.h"
#include "cli.h"
#include "simulator.h"

#include "cvts.h"
#include "tsrand48.h"

#include <stdlib.h>

/**
 * Restores the cursor visibility.
 */
void restore_cursor_visibility() {
    cvts_cursor_show();
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    assetmanager_init();
    atexit(restore_cursor_visibility);   // restore cursor visibility when the program terminates.

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

    printf("\n");
    const size_t its = cli_args.iterations;
    size_t* sides = malloc(game.die.sides.size * sizeof(*sides));
    if (!sides) {
        fprintf(stderr, "%serror:%s unable to simulate %lu dices.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), its);
        exit(1);
    }
    for (size_t side = 1; side != game.die.sides.size + 1; side++)
        sides[side - 1] = 0;
    printf("simulating %lu dices\n", its);
    tsnewseed48();
    size_t it = 0;
    for (; it < its; it++)
        sides[dice(&game.die) - 1]++;
    for (size_t side = 1; side != game.die.sides.size + 1; side++)
        printf("%s%3lu%s %12lu %10.6lf%%\n", FMT(FMTVAL_FG_BRIGHT_BLACK), side, FMT(FMTVAL_FG_DEFAULT), sides[side - 1], ((double)sides[side - 1] / (double)its) * 100.0);
    printf("\n");
    free(sides);

    simulate(&game, cli_args.iterations);

    assetmanager_free_all();
}
