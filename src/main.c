#include "assetmanager.h"
#include "cli.h"
#include "game.h"

#include "cvts.h"
#include "tsrand48.h"

#include <stdlib.h>

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
    for (size_t side = 0; side < game.die.sides.size; side++)
        sides[side] = 0;
    printf("simulating %lu dices\n", its);
    tsnewseed48();
    size_t it = 0;
    for (; it < its; it++)
        sides[dice(&game.die)]++;
    for (size_t side = 0; side < game.die.sides.size; side++)
        printf("%s%3lu%s %12lu %10.6lf%%\n", FMT(FMTVAL_FG_BRIGHT_BLACK), side, FMT(FMTVAL_FG_DEFAULT), sides[side], ((double)sides[side] / (double)its) * 100.0);
    free(sides);

    assetmanager_free_all();
}
