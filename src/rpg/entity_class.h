//
// Created by bison on 9/23/18.
//

#ifndef GAME_ENTITY_CLASS_H
#define GAME_ENTITY_CLASS_H

#include <defs.h>
#include <vector.h>

#define BASE_AC 10

typedef struct {
    i32         level;
    i32         minXP;
    i32         maxXP;
    char        hitDice[16];
} LevelRow;

typedef struct {
                char name[64];
                Vector levelTable;
} EntityClass;

void            EntityClass_InitFighter(EntityClass *entityClass);
void            EntityClass_Free(EntityClass *entityClass);

i32             EntityClass_CalcLevel(EntityClass *entityClass, i32 xp);
const char*     EntityClass_GetHitDice(EntityClass *entityClass, i32 level);
i32             EntityClass_GetAttackBonus(i32 level);
i32             EntityClass_GetXPForLevel(EntityClass* entityClass, i32 level);

#endif //GAME_ENTITY_CLASS_H
