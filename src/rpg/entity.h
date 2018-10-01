//
// Created by bison on 9/23/18.
//

#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "ability_score.h"
#include "entity_class.h"
#include "monster.h"
#include "armor.h"
#include "faction.h"
#include "rpg_defs.h"
#include "attack.h"

typedef enum                    {ET_CHARACTER, ET_MONSTER} EntityType;

typedef struct
{
    EntityType                  type;
    char                        name[RPG_STR_SIZE_MEDIUM];
    i32                         level;
    i32                         HP;
    i32                         maxHP;
    i32                         XP;
    i32                         money;
    AbilityScore                abilityScore;
    GrammarType                 grammarType;
    Faction*                    faction;
    Attack*                     attacks[RPG_MAX_ENTITY_ATTACKS];
    i32                         attackCount;

    union {
        // ET_CHARACTER
        struct {
            EntityClass*        entityClass;
            Armor*              armor;
            Armor*              shield;
            Weapon*             mainWeapon;
            Weapon*             offWeapon;
        };
        // ET_MONSTER
        struct {
            MonsterTemplate*    monsterTemplate;
        };
    };
} Entity;

bool    Entity_Init(Entity *entity, EntityType type, i32 level, const char* name, EntityClass *entityClass, MonsterTemplate* template);
void    Entity_GetHitDice(Entity *entity, char* hitdice, size_t size);
i32     Entity_GetAttackBonus(Entity *entity);
void    Entity_IncreaseLevel(Entity *entity);
bool    Entity_AwardXP(Entity *entity, int xp);
i32     Entity_GetAC(Entity* entity);
i32     Entity_GetDEXBonus(Entity *entity);
i32     Entity_GetSTRBonus(Entity *entity);
i32     Entity_GetCONBonus(Entity *entity);
void    Entity_SetArmor(Entity *entity, Armor* armor);
void    Entity_SetShield(Entity *entity, Armor* shield);
void    Entity_SetMainWeapon(Entity *entity, Weapon *weapon);
void    Entity_SetOffWeapon(Entity *entity, Weapon *weapon);
bool    Entity_IsDead(Entity* entity);
Attack* Entity_GetMaxRangedAttack(Entity* entity);

#endif
