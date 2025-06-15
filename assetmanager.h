#pragma once

/**
 * Asset manager with a global asset manager that can be used to reference values that should be freed at some point in the future.
 */

#include "array.h"

// forward declarations
typedef struct cli_args_t cli_args_t;

/**
 * Struct for the asset manager.
 */
typedef struct assetmanager_t {
    array_t cli_args;          // The array with pointers to all assets of type cli_args_t
} assetmanager_t;

// The global instance of the asset manager.
extern assetmanager_t assetmanager;

/**
 * Frees all assets of the given asset manager.
 * If no asset manager was given no action is performed.
 * @param assetmanager The asset manager whose assets should be freed.
 */
void assetmanager_free(assetmanager_t* assetmanager);

/**
 * Adds the address of the given cli_args to the asset manager.
 * @param assetmanager The asset manager cli_args should be added to.
 * @param cli_args The address of the cli_args_t instance that should be added to the asset manager
 * @return The address of the added asset pointer.
 */
cli_args_t** assetmanager_add_cli_args(assetmanager_t* assetmanager, cli_args_t* cli_args);
