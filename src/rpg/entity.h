//
// Created by bison on 9/23/18.
//

#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "ability_score.h"
#include "entity_class.h"

typedef enum        {ET_CHARACTER, ET_MONSTER} EntityType;

typedef struct
{
    EntityType      type;
    char            name[64];
    i32             level;
    i32             HP;
    i32             maxHP;
    i32             XP;
    i32             money;
    i32             lastInitiativeRoll;
    AbilityScore    abilityScore;
    EntityClass*    entityClass;
} Entity;

bool                Entity_Init(Entity *entity, EntityType type, i32 level, const char* name, EntityClass *entityClass);
void                Entity_GetHitDice(Entity *entity, char* hitdice, size_t size);
i32                 Entity_GetAttackBonus(Entity *entity);
void                Entity_IncreaseLevel(Entity *entity);
bool                Entity_AwardXp(Entity *entity, int xp);

#endif
