#include "array.h"

#include <stdlib.h>
#include <string.h>

array_t array_create(size_t initcapacity, size_t elementsize) {
    if (elementsize == 0)
        elementsize = 1;
    void* data = initcapacity != 0 ? malloc(initcapacity * elementsize) : 0;
    return data
        ? (array_t){ .elementsize = elementsize }
        : (array_t){
            .data = data,
            .capacity = initcapacity,
            .size = 0,
            .elementsize = elementsize
        };
}

void array_free_full(array_t* array) {
    if (!array)
        return;
    if (array->data)
        free(array->data);
    *array = (array_t){};
}

void array_free(array_t* array) {
    if (!array)
        return;
    if (array->data)
        free(array->data);
    array->data = 0;
    array->capacity = 0;
    array->size = 0;
}

void array_clear(array_t* array) {
    if (!array)
        return;
    array->size = 0;
}

bool array_isempty(array_t* array) {
    return array ? array->size == 0 : true;
}

bool array_reserve(array_t* array, size_t newcapacity) {
    if (!array)
        return false;
    if (array->capacity >= newcapacity)
        return true;
    if (array->elementsize == 0)
        array->elementsize = 1;
    void* newdata = array->data ? realloc(array->data, newcapacity * array->elementsize) : malloc(newcapacity * array->elementsize);
    if (!newdata)
        return false;
    array->data = newdata;
    array->capacity = newcapacity;
    return true;
}

void* array_get(array_t* array, size_t index) {
    if (!array || index >= array->size)
        return 0;
    if (array->elementsize == 0)
        array->elementsize = 1;
    return &array->data[index * array->elementsize];
}

void* array_set(array_t* array, size_t index, const void* element) {
    void* dst = array_get(array, index);
    if (!dst)
        return 0;
    memcpy(dst, element, array->elementsize);
    return dst;
}

void* array_add(array_t* array, const void* element) {
    if (!array || !element)
        return 0;
    if (!array->data || array->capacity == array->size) {
        size_t newcapacity = array->capacity < 2 ? 2 : array->capacity + array->capacity / 2;
        if (!array_reserve(array, newcapacity))
            return 0;
    }
    return array_set(array, array->size++, element);
}
