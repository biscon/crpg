//
// Created by bison on 9/24/18.
//

#ifndef GAME_RPG_H
#define GAME_RPG_H

#include "entity.h"

#define RPG_MAX_CLASSES 128

typedef struct {
    EntityClass*    entityClasses[RPG_MAX_CLASSES];
    i32             entityClassCount;
} RPGContext;


void                RPG_Init();
void                RPG_InitContext(RPGContext *context);
void                RPG_ShutdownContext(RPGContext *context);
EntityClass*        RPG_GetEntityClass(RPGContext *context, const char* classname);


#endif //GAME_RPG_H
