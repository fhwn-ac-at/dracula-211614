#include "cli.h"

int main(int argc, char* argv[]) {
    printf("args[%d] { ", argc);
    for (size_t i = 0; i < argc; i++)
        printf("\"%s\"%s ", argv[i], i != argc - 1 ? "," : "");
    printf("}\n");

    cli_args_t cli_args = cli_parse(argc, argv, false);
    cli_args_print(&cli_args);
}
