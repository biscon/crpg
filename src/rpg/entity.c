//
// Created by bison on 9/23/18.
//

#include <string.h>
#ifdef DEBUG
    #include <assert.h>
#endif
#include <stdio.h>
#include "entity.h"
#include "ability_score.h"
#include "dice.h"

internal void CharacterRollStartingMaxHP(Entity *entity)
{
    const char* hitdice = EntityClass_GetHitDice(entity->entityClass, entity->level);
#ifdef DEBUG
    assert(hitdice != NULL);
#endif
    i32 con_bonus = AbilityScore_CalcPenalty(entity->abilityScore.CON);
    i32 max_hp = Dice_Roll(hitdice);
    max_hp += con_bonus;
    if(max_hp < 1)
        max_hp = 1;
    max_hp += 8;
    entity->HP = max_hp;
    entity->maxHP = max_hp;
}

internal void MonsterRollStartingMaxHP(Entity *entity)
{
    i32 max_hp = 0;
    char hitdice[16];
    memset(hitdice, 0, sizeof(hitdice));
    Entity_GetHitDice(entity, hitdice, sizeof(hitdice));
    for(int i = 1; i <= entity->level; ++i)
    {
        i32 roll = Dice_Roll(hitdice);
        max_hp += roll;
    }
}

bool Entity_Init(Entity *entity, EntityType type, i32 level, const char *name, EntityClass *entityClass)
{
    memset(entity, 0, sizeof(Entity));
    entity->type = type;
    entity->entityClass = entityClass;
    entity->level = level;
    //memset(entity->name, 0, sizeof(entity->name));
    strncpy(entity->name, name, sizeof(entity->name));
    AbilityScore_Roll4D6DropLowest(&entity->abilityScore);

    switch(entity->type)
    {
        case ET_CHARACTER: {
            CharacterRollStartingMaxHP(entity);
            entity->money = 10 * Dice_Roll("3d6");
            break;
        }
        case ET_MONSTER: {
            MonsterRollStartingMaxHP(entity);
            break;
        }
    }

    return true;
}

void Entity_GetHitDice(Entity *entity, char* hitdice, size_t size) {
    if(entity->type == ET_MONSTER) {
        snprintf(hitdice, size, "%dd8", entity->level);
    } else if(entity->type == ET_CHARACTER) {
        strncpy(hitdice, EntityClass_GetHitDice(entity->entityClass, entity->level), size);
    }
}

i32 Entity_GetAttackBonus(Entity *entity) {
    switch(entity->type)
    {
        case ET_CHARACTER: {
            return EntityClass_GetAttackBonus(entity->level);
        }
        case ET_MONSTER: {
            i32 level = entity->level;
            if(level >= 0 && level <= 7)
                return level;
            if(level >= 8 && level <= 9)
                return 8;
            if(level >= 10 && level <= 11)
                return 9;
            if(level >= 12 && level <= 13)
                return 10;
            if(level >= 14 && level <= 15)
                return 11;
            if(level >= 16 && level <= 19)
                return 12;
            if(level >= 20 && level <= 23)
                return 13;
            if(level >= 24 && level <= 27)
                return 14;
            if(level >= 28 && level <= 31)
                return 15;
            if(level >= 21)
                return 16;
            break;
        }
    }
    return 0;
}

internal void RollLevelUpMaxHPBonus(Entity *entity)
{
    const char* hitdice = EntityClass_GetHitDice(entity->entityClass, entity->level);
#ifdef DEBUG
    assert(hitdice != NULL);
#endif
    i32 con_bonus = AbilityScore_CalcPenalty(entity->abilityScore.CON);
    i32 bonus = Dice_Roll(hitdice);
    bonus += con_bonus;
    if(bonus < 1)
        bonus = 1;

    entity->maxHP += bonus;
    entity->HP = entity->maxHP;
}

void Entity_IncreaseLevel(Entity *entity) {
    entity->level++;
    entity->XP = EntityClass_GetXPForLevel(entity->entityClass, entity->level);
    if(entity->type == ET_CHARACTER)
        RollLevelUpMaxHPBonus(entity);
}

// returns true if the entity gained a level
bool Entity_AwardXp(Entity *entity, int xp)
{
    if(entity->type != ET_CHARACTER)
        return false;

    i32 n_xp = entity->XP;
    n_xp += xp;
    i32 lvl = EntityClass_CalcLevel(entity->entityClass, n_xp);
    entity->XP = n_xp;
    if(lvl > entity->level)
    {
        entity->level++;
        RollLevelUpMaxHPBonus(entity);
        return true;
    }
    return false;
}


