//
// Created by bison on 12-05-2017.
//
#include <SDL_log.h>
#include <defs.h>
#include "store.h"

void Store_Init(Store* c, size_t item_size)
{
    c->noItems = 0;
    c->itemSize = item_size;
    c->capacity = CONTAINER_INITIAL_SIZE;
    c->data = calloc(1, CONTAINER_INITIAL_SIZE * item_size);
}

void Store_Destroy(Store *c)
{
    //SDL_Log("Destroying container");
    if(c->data)
    {
        free(c->data);
        c->data = NULL;
        c->capacity = 0;
        c->noItems = 0;
    }
}

INTERNAL void resizeContainer(Store *c)
{
    size_t old_cap = c->capacity;
    c->capacity = 2 * old_cap;
    //SDL_Log("Resizing container capacity from %ld to %ld items", old_cap, c->capacity);
    c->data = realloc(c->data, c->capacity * c->itemSize);
}

u32 Store_Pushback(Store *c, void *item)
{
    // copy data into container
    if(c->noItems >= c->capacity)
    {
        resizeContainer(c);
    }
    char *ptr = c->data + (c->noItems * c->itemSize);
    memcpy(ptr, item, c->itemSize);
    u32 index = c->noItems;
    c->noItems++;
    //SDL_Log("Adding item to container at index %d, new count %d", index, c->noItems);
    return index;
}

void Store_InsertItemAt(Store *c, u32 i, void *item)
{
    if(i == c->noItems-1)
    {
        Store_Pushback(c, item);
        return;
    }
    // copy data into container
    if(c->noItems >= c->capacity)
    {
        resizeContainer(c);
    }

    // make room in array for new item
    char *src_ptr = c->data + (i * c->itemSize);
    char *dst_ptr = src_ptr + c->itemSize;
    int copy_items = c->noItems - i;
    size_t move_size = copy_items * c->itemSize;
    //SDL_Log("Adding item at %d, container size is %d. Moving %d items one down, moving %ld bytes", i, c->noItems, copy_items, move_size);
    memmove(dst_ptr, src_ptr, move_size);
    memcpy(src_ptr, item, c->itemSize);
    c->noItems++;
    //SDL_Log("Adding item to container at index %d, new count %d", i, c->noItems);
}

u32 Store_GetSize(Store *c)
{
    return c->noItems;
}

void *Store_GetItemAt(Store *c, u32 index)
{
    if(index > c->noItems-1)
        return NULL;
    char *ptr = c->data + (index * c->itemSize);
    return (void *) ptr;
}

void Store_RemoveItemAt(Store *c, u32 index)
{
    if(index > c->noItems-1)
        return;
    char *ptr = c->data + (index * c->itemSize);
    // if not the last item
    if(index < c->noItems-1)
    {
        char *rest_ptr = ptr + c->itemSize;
        int rest_items = c->noItems - index - 1;
        size_t move_size =  rest_items * c->itemSize;
        //SDL_Log("Removing item at %d, container size is %d. Moving %d items one up, moving %ld bytes", index, c->noItems, rest_items, move_size);
        memmove(ptr, rest_ptr, move_size);
    }
    c->noItems--;
    //SDL_Log("Element at index %d has been removed, new container size is %d", index, c->noItems);
}

i32 Store_IndexOf(Store *c, void *ptr)
{
    for(i32 i = 0; i < c->noItems-1; i++)
    {
        if(c->data + (i * c->itemSize) == ptr)
            return i;
    }
    return -1;
}