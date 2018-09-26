//
// Created by bison on 9/24/18.
//

#ifndef GAME_RPG_H
#define GAME_RPG_H

#include "entity.h"
#include "armor.h"
#include "weapon.h"
#include "monster.h"

#define RPG_MAX_CLASSES             128
#define RPG_MAX_ARMOR_TEMPLATES     128
#define RPG_MAX_WEAPON_TEMPLATES    128
#define RPG_MAX_MONSTER_TEMPLATES   128

typedef struct {
    EntityClass*        entityClasses[RPG_MAX_CLASSES];
    i32                 entityClassCount;
    ArmorTemplate*      armorTemplates[RPG_MAX_ARMOR_TEMPLATES];
    i32                 armorTemplateCount;
    WeaponTemplate*     weaponTemplates[RPG_MAX_WEAPON_TEMPLATES];
    i32                 weaponTemplateCount;
    MonsterTemplate*    monsterTemplates[RPG_MAX_MONSTER_TEMPLATES];
    i32                 monsterTemplateCount;
} RPGContext;


void                RPG_Init();
void                RPG_InitContext(RPGContext *context);
void                RPG_ShutdownContext(RPGContext *context);
EntityClass*        RPG_GetEntityClass(RPGContext *context, const char* classname);
ArmorTemplate*      RPG_GetArmorTemplate(RPGContext *context, const char* templatename);
MonsterTemplate*    RPG_GetMonsterTemplate(RPGContext *context, const char* templatename);


#endif //GAME_RPG_H
