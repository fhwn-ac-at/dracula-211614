#pragma once

/**
 * Asset manager with a global asset manager that can be used to reference values that should be freed at some point in the future.
 */

#include "array.h"

/**
 * The function pointer type of a function used to deallocate the data at the given address.
 * @param data The address of the data that should be deallocated.
 */
typedef void(*deallocator_fn_t)(void* data);

/**
 * Struct for a single asset storing the address of the resource and the function pointer to the resource's deallocator function.
 */
typedef struct asset_t {
    void* data;                     // The managed data of the asset
    deallocator_fn_t deallocator;   // The deallocator used to deallocate the asset's data upon destruction.
} asset_t;

/**
 * Struct for the asset manager.
 * If an asset has no data when it is released the deallocator is still executed but is given 0 as data pointer.
 * If an asset has no deallocator when it is released the asset is removed from the manager but the data remains untouched.
 */
typedef struct assetmanager_t {
    array_t assets;             // The array of the managed asset_ts
} assetmanager_t;

// The global instance of the asset manager.
extern assetmanager_t assetmanager;

/**
 * Registers the assetmanager_free function as cleanup function with the atexit function.
 * If it could not be registered as exit function an appropriate error
 * message is output on stderr and the program is terminated with exit code 1.
 * @return 0 if the function was successfully registered as exit function, otherwise the program is terminated with exit code 1.
 */
int assetmanager_register();

/**
 * Frees all assets of the global asset manager.
 * If no asset manager was given no action is performed.
 */
void assetmanager_free();

/**
 * Adds the a new asset with the given data and deallocator to the global asset manager.
 * The when the asset is freed the data is deallocated with the given deallocator function.
 * A new asset is add even if no data or deallocator is given.
 * @param data The address of the asset data that should be added to the asset manager.
 * @param deallocator The deallocator that should be used to deallocate the asset data upon releasing the asset.
 * @return The address of the added asset, 0 if it could not be added.
 */
asset_t* assetmanager_add(void* data, deallocator_fn_t deallocator);

/**
 * Adds the given asset to the global asset manager.
 * @param asset The asset that should be added to the asset manager
 * @return The address of the added asset, 0 if it could not be added.
 */
asset_t* assetmanager_add_asset(asset_t asset);
