//
// Created by bison on 9/23/18.
//

#ifndef GAME_ENTITY_CLASS_H
#define GAME_ENTITY_CLASS_H

#include <defs.h>
#include <vector.h>
#include "rpg_defs.h"

typedef struct {
    i32         level;
    i32         minXP;
    i32         maxXP;
    char        hitDice[RPG_DICE_STR_SIZE];
} LevelRow;

typedef struct {
                char name[RPG_STR_SIZE_MEDIUM];
                Vector levelTable;
} EntityClass;

void            EntityClass_InitFighter(EntityClass *entityClass);
void            EntityClass_Free(EntityClass *entityClass);

i32             EntityClass_CalcLevel(EntityClass *entityClass, i32 xp);
const char*     EntityClass_GetHitDice(EntityClass *entityClass, i32 level);
i32             EntityClass_GetXPForLevel(EntityClass* entityClass, i32 level);

#endif //GAME_ENTITY_CLASS_H
