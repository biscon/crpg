//
// Created by bison on 9/23/18.
//

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

void VectorAlloc(Vector *v)
{
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
}

int VectorSize(Vector *v)
{
    return v->total;
}

void VectorResize(Vector *v, i32 capacity)
{
#ifdef DEBUG
    //printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif

    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void VectorAdd(Vector *v, void *item)
{
    if (v->capacity == v->total)
        VectorResize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void VectorSet(Vector *v, i32 index, void *item)
{
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}

void *VectorGet(Vector *v, i32 index)
{
    if (index >= 0 && index < v->total)
        return v->items[index];
    return NULL;
}

void VectorRemove(Vector *v, i32 index)
{
    if (index < 0 || index >= v->total)
        return;

    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        VectorResize(v, v->capacity / 2);
}

void VectorFree(Vector *v)
{
    free(v->items);
}

void VectorRemoveItem(Vector *v, void *item) {
    i32 index = -1;
    for(i32 i = 0; i < v->total; i++) {
        if(VectorGet(v, i) == item) {
            index = i;
            break;
        }
    }
    if(index > -1) {
        VectorRemove(v, index);
    }
}
