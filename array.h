#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * Struct for an array capable of holding elements of arbitrary size in a contiguous memory block.
 * The elementsize should not be changed while the array is not empty unless the next action on the array is a clear or free.
 * If elementsize == 0 when it is needed by any array function (e.g. reserve, get, set, add) it is set to 1 as default.
 */
typedef struct array_t {
    void* data;                 // A pointer to the beginning of the data (elements) memory block.
    size_t capacity;            // The number of elements that can be stored in the currently allocated data memory block.
    size_t size;                // The number of elements currently stored in the data memory block.
    size_t elementsize;         // The size of each element the array can store.
} array_t;

/**
 * Creates a new array with the given initial capacity and the given elementsize.
 * @param initcapacity The initial capacity of the array.
 * @param elementsize The size of a single element. If it equals 0 elementsize defaults to 1.
 * @return The newly created array.
 */
array_t array_create(size_t initcapacity, size_t elementsize);

/**
 * Frees the given array by deallocating it's memory buffer and setting it's capacity, size and elementsize to 0.
 * @param array The array that should be freed.
 */
void array_free_full(array_t* array);

/**
 * Frees the given array by deallocating it's memory buffer and setting it's capacity and size to 0. The elementsize remains unchanged.
 * @param array The array that should be freed.
 */
void array_free(array_t* array);

/**
 * Removes all elements from the array by setting it's size to 0. The memory buffer, capacity and elementsize remain unchanged.
 * @param array The array that should be cleared.
 */
void array_clear(array_t* array);

/**
 * Checks whether the array is empty.
 * @param array The array to check.
 * @return true if array->size == 0 or no array was given, false otherwise.
 */
bool array_isempty(array_t* array);

/**
 * Reserves a big enough data memory block to store newcapacity many elements in the array.
 * If the current capacity > newcapacity no action is performed. Hence this function can only upscale the memory block capacity but not downscale.
 * If a new memory block should be allocated but array->elementsize == 0 it is set to 1 as default.
 * @param array The array that should have at least the new capacity.
 * @param newcapacity The minimum capacity the array should have.
 * @return true if the reserve was successful or the array already had a big enough capacity, false otherwise.
 */
bool array_reserve(array_t* array, size_t newcapacity);

/**
 * Retrieves the address of the element at the given index in the array. 
 * If array->elementsize == 0 it is set to 1 as default.
 * @return The address of the element at the given index.
 */
void* array_get(array_t* array, size_t index);

/**
 * Sets the element at the given index in the array to the array->elementsize many bytes beginning at the given element address.
 * If array->elementsize == 0 it is set to 1 as default.
 * @return The address of the element at the given index.
 */
void* array_set(array_t* array, size_t index, const void* element);

/**
 * Adds a new element to the end of the array. The element data is array->elementsize many bytes beginning at the given data address.
 * The caller is responsible for providing a sufficiently sized memory block.
 * If necessary the memory buffer is scaled up by allocating a new memory buffer transferring all elements and adding a copy of the new one at the end.
 * If the element could not be added to the array no action is performed and 0 is returned.
 * @param array The array the element should be added to.
 * @param element The address of the first byte of the element that should be added to the array.
 * @return The address of the newly added element, 0 if it could not be added.
 */
void* array_add(array_t* array, const void* element);
