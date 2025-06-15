#include "assetmanager.h"

#include "cli.h"

assetmanager_t assetmanager = {
    .cli_args = (array_t){ 0, sizeof(cli_args_t*), 0 }
};

void assetmanager_free(assetmanager_t* assetmanager) {
    if (!assetmanager)
        return;
    for (size_t i = 0; i < assetmanager->cli_args.size; i++)
        cli_args_free(array_get(&assetmanager->cli_args, i));
    array_free(&assetmanager->cli_args);
}

cli_args_t** assetmanager_add_cli_args(assetmanager_t* assetmanager, cli_args_t* cli_args) {
    return assetmanager && cli_args ? array_add(&assetmanager->cli_args, &cli_args) : 0;
}
