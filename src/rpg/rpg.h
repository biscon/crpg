//
// Created by bison on 9/24/18.
//

#ifndef GAME_RPG_H
#define GAME_RPG_H

#include "entity.h"
#include "armor.h"
#include "weapon.h"
#include "monster.h"
#include "rpg_defs.h"
#include "faction.h"

typedef struct {
    EntityClass*        entityClasses[RPG_MAX_CLASSES];
    i32                 entityClassCount;
    ArmorTemplate*      armorTemplates[RPG_MAX_ARMOR_TEMPLATES];
    i32                 armorTemplateCount;
    WeaponTemplate*     weaponTemplates[RPG_MAX_WEAPON_TEMPLATES];
    i32                 weaponTemplateCount;
    MonsterTemplate*    monsterTemplates[RPG_MAX_MONSTER_TEMPLATES];
    i32                 monsterTemplateCount;
    Faction*            factions[RPG_MAX_FACTIONS];
    i32                 factionCount;
} RPGContext;


void                    RPG_Init();
void                    RPG_InitContext(RPGContext *context);
void                    RPG_ShutdownContext(RPGContext *context);
EntityClass*            RPG_GetEntityClass(RPGContext *context, const char* classname);

// Characters
Entity*                 RPG_CreateCharacterEntity(RPGContext* context, EntityClass *entityClass,
                                         const char* name, i32 level);

// Monsters
MonsterTemplate*        RPG_GetMonsterTemplate(RPGContext *context, const char* templatename);
Entity*                 RPG_CreateMonsterFromTemplate(RPGContext *context, MonsterTemplate *template, i32 level);
void                    RPG_DestroyEntity(Entity* entity);
void                    RPG_LogEntity(Entity* entity);

// Armor
ArmorTemplate*          RPG_GetArmorTemplate(RPGContext *context, const char* templatename);

// Weapons
WeaponTemplate*         RPG_GetWeaponTemplate(RPGContext *context, const char* templatename);

// Faction

#endif //GAME_RPG_H
