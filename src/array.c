#include "array.h"

#include <stdlib.h>
#include <string.h>

int byte_compare(size_t valuesize, const void* a, const void* b) {
    if (!a || !b)
        return a || b ? a ? 1 : -1 : 0;
    for (size_t i = 0; i < valuesize; i++)
        if (((char*)a)[i] != ((char*)b)[i])
            return ((char*)a)[i] - ((char*)b)[i];
    return 0;
}

int array_value_compare(array_t* array, const void* a, const void* b) {
    return !array
        ? byte_compare(1, a, b)
        : (array->comparator ? array->comparator : byte_compare)(array->elementsize, a, b);
}

array_t array_create(size_t initcapacity, size_t elementsize, comparator_fn_t comparator) {
    if (elementsize == 0)
        return (array_t){ .comparator = comparator };
    void* data = initcapacity != 0 ? malloc(initcapacity * elementsize) : 0;
    return data
        ? (array_t){ .data = data, .capacity = initcapacity, .size = 0, .elementsize = elementsize, .comparator = comparator }
        : (array_t){ .elementsize = elementsize, .comparator = comparator };
}

void array_free_full(array_t* array) {
    if (!array)
        return;
    if (array->data)
        free(array->data);
    *array = (array_t){};
}

void array_free(array_t* array, element_fn_t destructor) {
    if (!array)
        return;
    if (array->data) {
        if (destructor)
            for (size_t i = 0; i < array->size; i++)
                destructor(array_get(array, i));
        free(array->data);
    }
    array->data = 0;
    array->capacity = 0;
    array->size = 0;
}

void array_clear(array_t* array) {
    if (!array)
        return;
    array->size = 0;
}

bool array_isempty(const array_t* array) {
    return array ? array->size == 0 : true;
}

bool array_reserve(array_t* array, size_t newcapacity) {
    if (!array || array->elementsize == 0)
        return false;
    if (array->capacity >= newcapacity)
        return true;
    void* newdata = array->data ? realloc(array->data, newcapacity * array->elementsize) : malloc(newcapacity * array->elementsize);
    if (!newdata)
        return false;
    array->data = newdata;
    array->capacity = newcapacity;
    return true;
}

void* array_get(array_t* array, size_t index) {
    if (!array || index >= array->size || array->elementsize == 0)
        return 0;
    return &array->data[index * array->elementsize];
}

const void* array_getconst(const array_t* array, size_t index) {
    if (!array || index >= array->size || array->elementsize == 0)
        return 0;
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

void* array_find(array_t* array, const void* element, comparator_fn_t comparator) {
    if (!array || !element)
        return 0;
    for (size_t i = 0; i < array->size; i++) {
        void* el = array_get(array, i);
        if ((comparator ? comparator(array->elementsize, element, el) : array_value_compare(array, element, el)) == 0)
            return el;
    }
    return 0;
}

bool array_contains(array_t* array, const void* element, comparator_fn_t comparator) {
    return array_find(array, element, comparator);
}

bool array_rmv(array_t* array, size_t index) {
    if (!array || index >= array->size)
        return false;
    for (; index + 1 < array->size; index++)
        array_set(array, index, array_get(array, index + 1));
    memset(array_get(array, index), 0, array->elementsize);
    array->size--;
    return true;
}

array_t* array_copy(array_t* dst, const array_t* src) {
    if (!dst || !src || !array_reserve(dst, src->size))
        return 0;
    array_clear(dst);
    for (size_t i = 0; i < src->size; i++)
        if (!array_add(dst, array_getconst(src, i)))
            return 0;
    return dst;
}

array_t* array_move(array_t* dst, array_t* src) {
    if (!dst || !src)
        return 0;
    array_free(dst, 0);
    *dst = *src;
    src->capacity = 0;
    src->size = 0;
    src->data = 0;
    return dst;
}
