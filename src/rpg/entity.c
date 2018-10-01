//
// Created by bison on 9/23/18.
//

#include <string.h>
#ifdef DEBUG
    #include <assert.h>
#endif
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "entity.h"
#include "ability_score.h"
#include "dice.h"
#include "rpg_log.h"
#include "weapon.h"
#include "monster.h"
#include "../util/string_util.h"
#include "armor.h"
#include "attack.h"

internal void CharacterRollStartingMaxHP(Entity *entity)
{
    const char* hitdice = EntityClass_GetHitDice(entity->entityClass, entity->level);
#ifdef DEBUG
    assert(hitdice != NULL);
#endif
    i32 con_bonus = AbilityScore_CalcBonusOrPenalty(entity->abilityScore.CON);
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
    char hitdice[RPG_DICE_STR_SIZE];
    memset(hitdice, 0, sizeof(hitdice));
    Entity_GetHitDice(entity, hitdice, sizeof(hitdice));
    for(int i = 1; i <= entity->level; ++i)
    {
        i32 roll = Dice_Roll(hitdice);
        max_hp += roll;
    }
    entity->HP = max_hp;
    entity->maxHP = max_hp;
}

internal void ClearEntityAttacks(Entity *entity)
{
    if(entity->attackCount > 0) {
        for(i32 i = 0; i < entity->attackCount; ++i) {
            free(entity->attacks[i]);
        }
    }
    entity->attackCount = 0;
    memset(entity->attacks, 0, sizeof(entity->attacks));
}

// TODO break this up into subfunctions parsing attacks and damage rolls seperately to improve readability
internal void BuildMonsterEntityAttacks(Entity *entity)
{
    assert(entity->type == ET_MONSTER);
    assert(entity->monsterTemplate != NULL);

    // Parse attacks
    char str[RPG_STR_SIZE_LARGE] = {0};
    strncpy(str, entity->monsterTemplate->attacks, sizeof(str));
    char *end_str;
    char *token = strtok_r(str, "/", &end_str);
    char *parsed_tokens[64] = {0};
    i32 token_count = 0;
    while (token != NULL)
    {
        char *end_token;
        //printf("a = %s\n", token);
        char *token2 = strtok_r(token, " ", &end_token);
        while (token2 != NULL)
        {
            //printf("b = %s\n", token2);
            parsed_tokens[token_count] = strdup(token2);
            token_count++;
            token2 = strtok_r(NULL, " ", &end_token);
        }
        token = strtok_r(NULL, "/", &end_str);
    }
    RPG_LOG("Parsed %d tokens\n", token_count);
    assert(token_count % 2 == 0); // even amount of tokens


    // Parse damage rolls
    char *parsed_rolls[64] = {0};
    i32 roll_count = 0;
    char *next_token;
    char *buf = entity->monsterTemplate->damageDices;
    next_token = strtok (buf,"/ ");
    while (next_token != NULL)
    {
        parsed_rolls[roll_count] = strdup(next_token);
        roll_count++;
        next_token = strtok (NULL, "/ ");
    }

    RPG_LOG("Parsed %d rolls\n", roll_count);

    // process attacks
    i32 j = 0;
    for(i32 i = 0; i < token_count; i+= 2)
    {
        i64 times = strtol(parsed_tokens[i], NULL, 10);
        char *name = parsed_tokens[i+1];
        RPG_Log("Times = %d, name = %s\n", times, name);
        for(i32 u = 0; u < times; ++u) {
            Attack *a = calloc(1, sizeof(Attack));
            Attack_Init(a, AT_MONSTER, name, parsed_rolls[j], false, 0, NULL);
            entity->attacks[entity->attackCount] = a;
            entity->attackCount++;
        }
        ++j;
    }


    // free parsed tokens
    for(i32 i = 0; i < token_count; ++i)
    {
        free(parsed_tokens[i]);
    }

    // free parsed rolls
    for(i32 i = 0; i < roll_count; ++i)
    {
        free(parsed_rolls[i]);
    }
}

internal void BuildCharacterEntityAttacks(Entity *entity)
{
    // character is unarmed
    if(entity->offWeapon == NULL && entity->mainWeapon == NULL) {
        Attack* a = calloc(1, sizeof(Attack));
        Attack_Init(a, AT_MAIN_HAND, "fists", "1d3", false, 0, NULL);
        entity->attacks[entity->attackCount] = a;
        entity->attackCount++;
        return;
    }

    // if we have a main hand add attack
    if(entity->mainWeapon != NULL) {
        WeaponTemplate* template = entity->mainWeapon->template;
        assert(template != NULL);
        Attack* a = calloc(1, sizeof(Attack));
        AttackType attackType;
        if(template->size == WS_LARGE) {
            attackType = AT_TWO_HAND;
        }
        else
            attackType = AT_MAIN_HAND;

        Attack_Init(a, attackType, template->name, template->damage, template->range > 0, template->range, entity->mainWeapon);
        entity->attacks[entity->attackCount] = a;
        entity->attackCount++;
    }

    // same with offhand
    if(entity->offWeapon != NULL) {
        WeaponTemplate* template = entity->offWeapon->template;
        assert(template != NULL);
        Attack* a = calloc(1, sizeof(Attack));


        Attack_Init(a, AT_OFF_HAND, template->name, template->damage, template->range > 0, template->range, entity->offWeapon);
        entity->attacks[entity->attackCount] = a;
        entity->attackCount++;
    }
}

internal void RebuildEntityAttacks(Entity *entity)
{
    ClearEntityAttacks(entity);
    if(entity->type == ET_CHARACTER) {
        BuildCharacterEntityAttacks(entity);
    } else if(entity->type == ET_MONSTER) {
        BuildMonsterEntityAttacks(entity);
    }
}

bool Entity_Init(Entity *entity, EntityType type, i32 level, const char *name, EntityClass *entityClass, MonsterTemplate* template)
{
    //memset(entity, 0, sizeof(Entity));
    entity->type = type;
    if(type == ET_MONSTER)
        entity->monsterTemplate = template;
    if(type == ET_CHARACTER)
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

    RebuildEntityAttacks(entity);
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
    i32 level = entity->level;
    switch(entity->type)
    {
        case ET_CHARACTER: {
            if(level == 1)
                return 1;
            else if(level >=2 && level <=3)
                return 2;
            else if(level == 4)
                return 3;
            else if(level >= 5 && level <= 6)
                return 4;
            else if(level == 7)
                return 5;
            else if(level >= 8 && level <= 10)
                return 6;
            else if(level >= 11 && level <= 12)
                return 7;
            else if(level >= 13 && level <= 15)
                return 8;
            else if(level >= 16 && level <= 17)
                return 9;
            else if(level >= 18 && level <= 20)
                return 10;
            break;
        }
        case ET_MONSTER: {

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
    i32 con_bonus = AbilityScore_CalcBonusOrPenalty(entity->abilityScore.CON);
    i32 bonus = Dice_Roll(hitdice);
    bonus += con_bonus;
    if(bonus < 1)
        bonus = 1;

    entity->maxHP += bonus;
    entity->HP = entity->maxHP;
    RPG_LOG("Rolling level up HP bonus for entity %s. Bonus is %d, new max HP is %d\n", entity->name, bonus, entity->maxHP);
}

void Entity_IncreaseLevel(Entity *entity) {
    entity->level++;
    entity->XP = EntityClass_GetXPForLevel(entity->entityClass, entity->level);
    if(entity->type == ET_CHARACTER)
        RollLevelUpMaxHPBonus(entity);
    RPG_LOG("Increased level of entity %s to %d\n", entity->name, entity->level);
}

// returns true if the entity gained a level
bool Entity_AwardXP(Entity *entity, int xp)
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
        RPG_LOG("Entity %s gained %d XP and gained a level (lvl %d)\n", entity->name, xp, entity->level);
        return true;
    }
    else
        RPG_LOG("Entity %s gained %d XP\n", entity->name, xp);
    return false;
}

i32 Entity_GetAC(Entity *entity) {
    if(entity->type == ET_CHARACTER) {
        i32 dex_bonus = Entity_GetDEXBonus(entity);
        i32 ac = RPG_BASE_AC + dex_bonus;
        if (entity->armor != NULL)
            ac = entity->armor->template->AC + dex_bonus;
        if (entity->shield != NULL)
            ac += entity->shield->template->AC;
        return ac;
    } else if(entity->type == ET_MONSTER) {
        return entity->monsterTemplate->AC;
    }
    return RPG_BASE_AC;
}

i32 Entity_GetDEXBonus(Entity *entity)
{
    if(entity->type == ET_CHARACTER) {
        assert(entity->entityClass != NULL);
        return AbilityScore_CalcBonusOrPenalty(entity->abilityScore.DEX);
    }
    return 0;
}

i32 Entity_GetSTRBonus(Entity *entity)
{
    if(entity->type == ET_CHARACTER) {
        assert(entity->entityClass != NULL);
        return AbilityScore_CalcBonusOrPenalty(entity->abilityScore.STR);
    }
    return 0;
}

i32 Entity_GetCONBonus(Entity *entity)
{
    if(entity->type == ET_CHARACTER) {
        assert(entity->entityClass != NULL);
        return AbilityScore_CalcBonusOrPenalty(entity->abilityScore.CON);
    }
    return 0;
}

void Entity_SetArmor(Entity *entity, Armor *armor)
{
    assert(entity != NULL);
    assert(entity->type == ET_CHARACTER);
    entity->armor = armor;
}


void Entity_SetShield(Entity *entity, Armor *shield)
{
    assert(entity != NULL);
    assert(entity->type == ET_CHARACTER);
    entity->shield = shield;
}

void Entity_SetMainWeapon(Entity *entity, Weapon *weapon)
{
    assert(entity != NULL);
    entity->mainWeapon = weapon;
    if(weapon->template->size == WS_LARGE && entity->offWeapon != NULL) {
        RPG_LOG("Main hand weapon %s too large for dualwield, unequipping %s from offhand\n",
        weapon->template->name, entity->offWeapon->template->name);
    }
    RPG_LOG("Entity %s equipped weapon %s in main hand\n", entity->name, weapon->template->name);
    RebuildEntityAttacks(entity);
}

void Entity_SetOffWeapon(Entity *entity, Weapon *weapon) {
    assert(entity != NULL);
    if(weapon->template->size == WS_MEDIUM || weapon->template->size == WS_SMALL) {
        entity->offWeapon = weapon;
        RPG_LOG("Entity %s equipped weapon %s in offhand\n", entity->name, weapon->template->name);
    }
    else {
        RPG_LOG("Entity %s could not equip weapon %s in offhand.\n", entity->name, weapon->template->name);
    }
    RebuildEntityAttacks(entity);
}

bool Entity_IsDead(Entity *entity) {
    if(entity->HP <= 0)
        return true;
    return false;
}

Attack* Entity_GetMaxRangedAttack(Entity *entity) {
    i32 index_of_max = 0;
    i32 value_of_max = INT32_MIN;
    if(entity->attackCount > 0) {
        for(i32 i = 0; i < entity->attackCount; ++i) {
            Attack *attack = entity->attacks[i];
            if(attack->range > value_of_max) {
                index_of_max = i;
                value_of_max = attack->range;

            }
        }
        return entity->attacks[index_of_max];
    }
    return NULL;
}
