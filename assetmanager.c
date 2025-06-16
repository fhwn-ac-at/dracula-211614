#include "assetmanager.h"

#include "cli.h"
#include "cvts.h"

#include <stdlib.h>

assetmanager_t assetmanager = {
    .assets = (array_t){ 0, sizeof(asset_t), 0 }
};

int assetmanager_register() {
    if (atexit(assetmanager_free) != 0) {
        fprintf(stderr, "%serror:%s unable to register asset manager.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        exit(1);
    }
    return 0;
}

void assetmanager_free() {
    for (size_t i = 0; i < assetmanager.assets.size; i++) {
        asset_t* asset = array_get(&assetmanager.assets, i);
        if (asset->deallocator)
            asset->deallocator(asset->data);
    }
    array_free(&assetmanager.assets);
}

asset_t* assetmanager_add(void* data, deallocator_fn_t deallocator) {
    return array_add(&assetmanager.assets, &(asset_t){ data, deallocator });
}

asset_t* assetmanager_add_asset(asset_t asset) {
    return array_add(&assetmanager.assets, &asset);
}
