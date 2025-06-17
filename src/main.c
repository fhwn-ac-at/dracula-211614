#include "assetmanager.h"
#include "cli.h"
#include "game.h"

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

    assetmanager_free_all();
}
