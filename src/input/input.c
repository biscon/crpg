//
// Created by bison on 4/3/19.
//

#include <store.h>
#include "input.h"

INTERNAL Store mappingStore;

void Input_Init()
{
    STORE_INIT(mappingStore, sizeof(InputMapping));
}

void Input_Shutdown()
{
    STORE_DESTROY(mappingStore);
}

void Input_CreateMapping(InputMapping* mapping)
{
    STORE_PUSHBACK(mappingStore, mapping);
}
