#include "assetmanager.h"
#include "cli.h"
#include "game.h"

int main(int argc, char* argv[]) {
    assetmanager_register();

    printf("args[%d] { ", argc);
    for (size_t i = 0; i < argc; i++)
        printf("\"%s\"%s ", argv[i], i != argc - 1 ? "," : "");
    printf("}\n");

    cli_args_t cli_args = cli_parse(argc, argv);
    cli_args_print(&cli_args);

    game_t game = game_setup(&cli_args);
    printf("game adjmat\n");
    adjmat_print(&game.adjmat);

    printf("game\n");
    game_print(&game);
}
