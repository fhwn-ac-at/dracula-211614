#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * Function pointer type of a comparator function that compares the two values a and b which are both of size valuesize.
 * @param valuesize The size of the given values.
 * @param a The first value.
 * @param b The second value.
 * @return 0 if the values are considered equal, a negative value if a < b, a positive value if a > b.
 */
typedef int(*comparator_fn_t)(size_t valuesize, const void* a, const void* b);

/**
 * Function pointer type of a function that performs some action with the given array element.
 * @param element The element some action should be performed on.
 */
typedef void (*element_fn_t)(void* element);

/**
 * Struct for an array capable of holding elements of arbitrary size in a contiguous memory block.
 * The elementsize should not be changed while the array is not empty unless the next action on the array is a clear or free.
 * If a comparison is required (e.g. in array_find) the comparator function is used, if it is 0 the byte_compare function is used as a fallback.
 */
typedef struct array_t {
    void* data;                     // A pointer to the beginning of the data (elements) memory block.
    size_t capacity;                // The number of elements that can be stored in the currently allocated data memory block.
    size_t size;                    // The number of elements currently stored in the data memory block.
    size_t elementsize;             // The size of each element the array can store.
    comparator_fn_t comparator;     // The function used for value comparisons.
} array_t;

/**
 * Compares the two values a and b which are both of size valuesize.
 * If both values have the same sequence of bytes they are considered equal (return 0).
 * If the first non-equal byte is smaller in a than the byte in b a is considered smaller than b (return negative value).
 * If the first non-equal byte is greater in a than the byte in b a is considered greater than b (return positive value).
 * @param valuesize The size of the given values.
 * @param a The first value.
 * @param b The second value.
 * @return 0 if the values are considered equal, a negative value if a < b, a positive value if a > b.
 */
int byte_compare(size_t valuesize, const void* a, const void* b);

/**
 * Compares the two values a and b which are both of size array->elementsize with the arrays value comparator.
 * If array->comparator is not set or no array was given the byte_compare function is used for comparison instead.
 * If no array was given the value size defaults to 1.
 * @param array The array who's elementsize should be used in the comparator function.
 * @param a The first value.
 * @param b The second value.
 * @return 0 if the values are considered equal, a negative value if a < b, a positive value if a > b.
 */
int array_value_compare(array_t* array, const void* a, const void* b);

/**
 * Creates a new array with the given initial capacity, elementsize and value comparator.
 * If initial capacity is > 0 but elementsize is 0 no memory buffer is allocated and the capacity is set to 0.
 * @param initcapacity The initial capacity of the array.
 * @param elementsize The size of a single element.
 * @param comparator The comparator to use for value comparisons in this array.
 * @return The newly created array.
 */
array_t array_create(size_t initcapacity, size_t elementsize, comparator_fn_t comparator);

/**
 * Frees the given array by deallocating it's memory buffer and setting it's capacity, size, elementsize and comparator to 0.
 * @param array The array that should be freed.
 */
void array_free_full(array_t* array);

/**
 * Frees the given array by executing the given destructor function for each element,
 * deallocating the array's memory buffer and setting it's capacity and size to 0.
 * The elementsize and comparator remain unchanged.
 * If no destructor was given no action is performed for each element before buffer deallocation.
 * @param array The array that should be freed.
 * @param destructor The destructor that should be run for each element before freeing the buffer.
 */
void array_free(array_t* array, element_fn_t destructor);

/**
 * Removes all elements from the array by setting it's size to 0. The memory buffer, capacity and elementsize remain unchanged.
 * @param array The array that should be cleared.
 */
void array_clear(array_t* array);

/**
 * Checks whether the array is empty, i.e. the array's size is 0.
 * @param array The array to check.
 * @return true if the array is empty or no array was given, false otherwise.
 */
bool array_isempty(const array_t* array);

/**
 * Reserves a big enough data memory block to store newcapacity many elements in the array.
 * If the current capacity > newcapacity no action is performed. Hence this function can only upscale the memory block capacity but not downscale.
 * @param array The array that should have at least the new capacity.
 * @param newcapacity The minimum capacity the array should have.
 * @return true if the reserve was successful or the array already had a big enough capacity, false otherwise.
 */
bool array_reserve(array_t* array, size_t newcapacity);

/**
 * Retrieves the address of the element at the given index in the array.
 * @return The address of the element at the given index, 0 if the element does not exist.
 */
void* array_get(array_t* array, size_t index);

/**
 * Retrieves the address of the element at the given index in the array as a const pointer.
 * @return The address of the element at the given index as a const pointer, 0 if the element does not exist.
 */
const void* array_getconst(const array_t* array, size_t index);

/**
 * Sets the element at the given index in the array to the array->elementsize many bytes beginning at the given element address.
 * If no array or element was given or the array has no element at the given index no action is performed.
 * @return The address of the element at the given index.
 */
void* array_set(array_t* array, size_t index, const void* element);

/**
 * Adds a new element to the end of the array. The element data is array->elementsize many bytes beginning at the given data address.
 * The caller is responsible for providing a sufficiently sized memory block.
 * If necessary the memory buffer is scaled up by allocating a new memory buffer transferring all elements and adding a copy of the new element at the end.
 * If the element could not be added to the array no action is performed and 0 is returned.
 * @param array The array the element should be added to.
 * @param element The address of the first byte of the element that should be added to the array.
 * @return The address of the newly added element, 0 if it could not be added.
 */
void* array_add(array_t* array, const void* element);

/**
 * Finds the first element in the array that equals the given element according to either the
 * given comparator if given or the array's value comparator function if no comparator was given.
 * If no comparator was given and the array's value comparator is not set the byte_compare function is used as a fallback.
 * @param array The array to search the element in.
 * @param element The address of the value that should be found.
 * @param comparator The comparator function that is used to compare the given element with each element contained in the array.
 * @return The address of the found element in the array, 0 if no element in the array equaled the given element or no element or array was given.
 */
void* array_find(array_t* array, const void* element, comparator_fn_t comparator);

/**
 * Checks whether the given element is contained in the array. Equality of elements is either checked
 * with the given comparator if given or the array's value comparator function if no comparator was given.
 * If no comparator was given and the array's value comparator is not set the byte_compare function is used as a fallback.
 * @param array The array to search the element in.
 * @param element The address of the value that should be found.
 * @param comparator The comparator function that is used to compare the given element with each element contained in the array.
 * @return true if the element was found in the array, false if no element in the array equaled the given element or no element or array was given.
 */
bool array_contains(array_t* array, const void* element, comparator_fn_t comparator);

/**
 * Removes the element at the given index from the array.
 * All following elements are shifted 1 slot towards the beginning of the array.
 * Previously acquired pointers to all of those elements now are invalid / point to an incorrect element.
 * If no array was given or the array has no element at the given index no action is performed.
 * @return true if the element was removed successfully, false otherwise.
 */
bool array_rmv(array_t* array, size_t index);
