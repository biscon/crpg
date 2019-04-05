//
// Created by bison on 9/23/18.
//

#ifndef VECTOR_H
#define VECTOR_H
#include <defs.h>


#define VECTOR_INIT_CAPACITY 4

#define VECTOR_ALLOC(vec) Vector vec; VectorAlloc(&vec)
#define VECTOR_ADD(vec, item) VectorAdd(&vec, (void *) item)
#define VECTOR_SET(vec, id, item) VectorSet(&vec, id, (void *) item)
#define VECTOR_GET(vec, type, id) (type) VectorGet(&vec, id)
#define VECTOR_REMOVE(vec, id) VectorRemove(&vec, id)
#define VECTOR_REMOVE_ITEM(vec, item) VectorRemoveItem(&vec, (void *) item)
#define VECTOR_SIZE(vec) VectorSize(&vec)
#define VECTOR_FREE(vec) VectorFree(&vec)

typedef struct vector_t {
    void **items;
    int capacity;
    int total;
} Vector;

void VectorAlloc(Vector *v);
int VectorSize(Vector *v);
void VectorResize(Vector *v, i32 index);
void VectorAdd(Vector *v, void *item);
void VectorSet(Vector *v, i32 index, void *item);
void *VectorGet(Vector *v, i32 index);
void VectorRemove(Vector *v, i32 index);
void VectorRemoveItem(Vector *v, void *item);
void VectorFree(Vector *v);

#endif

