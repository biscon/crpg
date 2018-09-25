//
// Created by bison on 9/24/18.
//

#include <string.h>
#include <malloc.h>
#include "rpg.h"
#include "dice.h"
#include "entity_class.h"

void RPG_InitContext(RPGContext *context)
{
    memset(context, 0, sizeof(RPGContext));
    context->entityClasses[context->entityClassCount] = calloc(1, sizeof(EntityClass));
    EntityClass_InitFighter(context->entityClasses[context->entityClassCount]);
    context->entityClassCount++;
}

void RPG_ShutdownContext(RPGContext *context)
{
    for(i32 i = 0; i < context->entityClassCount; ++i) {
        free(context->entityClasses[i]);
    }
}

void RPG_Init()
{
    Dice_Init();
}

EntityClass *RPG_GetEntityClass(RPGContext *context, const char *classname)
{
    for(i32 i = 0; i < context->entityClassCount; ++i) {
        EntityClass *entityClass = context->entityClasses[i];
        if(strcmp(classname, entityClass->name) == 0) {
            return entityClass;
        }
    }
    return NULL;
}
