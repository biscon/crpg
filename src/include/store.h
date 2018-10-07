//
// Created by bison on 12-05-2017.
//

#ifndef GAME_STORE_H
#define GAME_STORE_H

#define CONTAINER_INITIAL_SIZE 16
#include <defs.h>

#define STORE_INIT(store, item_size)        Store_Init(&(store), (item_size))
#define STORE_DESTROY(store)                Store_Destroy(&(store))
#define STORE_PUSHBACK(store, item)         Store_Pushback(&(store), item)
#define STORE_SIZE(store)                   Store_GetSize(&(store))
#define STORE_GET_AT(store, index)          Store_GetItemAt(&(store), (index))
#define STORE_REMOVE_AT(store, i)           Store_RemoveItemAt(&(store), i)
#define STORE_INSERT_AT(store, i, item)     Store_InsertItemAt(&(store), i, item)
#define STORE_INDEX_OF(store, item)         Store_IndexOf(&(store), (item))

typedef struct Store Store;

struct Store
{
    u32     noItems;
    size_t  itemSize;
    size_t  capacity;
    void*   data;
};

void        Store_Init(Store* c, size_t item_size);
void        Store_Destroy(Store *c);
u32         Store_Pushback(Store *c, void *item);
u32         Store_GetSize(Store *c);
void*       Store_GetItemAt(Store *c, u32 index);
void        Store_RemoveItemAt(Store *c, u32 index);
void        Store_InsertItemAt(Store *c, u32 i, void *item);
i32         Store_IndexOf(Store *c, void *ptr);

#endif