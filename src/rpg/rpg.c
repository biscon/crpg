//
// Created by bison on 9/24/18.
//

#include <string.h>
#include <malloc.h>
#include "rpg.h"
#include "dice.h"
#include "entity_class.h"
#include "monster.h"
#include "rpg_defs.h"
#include "rpg_log.h"
#include "entity.h"

/*
 * Helper functions for creating armor templates
 */
internal void AddArmorTemplate(RPGContext* context, ArmorType type, const char *name, i32 ac, i32 weight, i32 cost)
{
    RPG_LOG("Loading armor template %s\n", name);
    ArmorTemplate *template = calloc(1, sizeof(ArmorTemplate));
    ArmorTemplate_Init(template, type, name, ac, weight, cost);
    context->armorTemplates[context->armorTemplateCount] = template;
    context->armorTemplateCount++;
}

internal void CreateArmorTemplates(RPGContext* context)
{
    #include "templates/armor_templates.inc"
    RPG_LOG("Loaded %d armor templates.\n", context->armorTemplateCount);
}

/*
 * Helper functions for creating weapon templates
 */
internal void AddWeaponTemplate(RPGContext* context, const char* name, i32 cost,
                                WeaponSize size, i32 weight, const char* damage, i32 range,
                                WeaponCategory cat)
{
    RPG_LOG("Loading weapon template %s\n", name);
    WeaponTemplate *template = calloc(1, sizeof(WeaponTemplate));
    WeaponTemplate_Init(template, name, cost, size, weight, damage, range, cat);
    context->weaponTemplates[context->weaponTemplateCount] = template;
    context->weaponTemplateCount++;
}

internal void CreateWeaponTemplates(RPGContext* context)
{
    #include "templates/weapon_templates.inc"
    RPG_LOG("Loaded %d weapon templates.\n", context->weaponTemplateCount);
}

/*
 * Helper functions for creating monster templates
 */
internal void AddMonsterTemplate(RPGContext* context, const char *name, i32 ac,
                                 const char *attacks, const char *dmgdices, i32 xp,
                                 const char *weapontpl, const char *factiontpl, const char *aicls)
{
    RPG_LOG("Loading monster template %s\n", name);
    MonsterTemplate *template = calloc(1, sizeof(MonsterTemplate));
    MonsterTemplate_Init(template, name, ac, attacks, dmgdices, xp, weapontpl, factiontpl, aicls, GT_CREATURE);
    context->monsterTemplates[context->monsterTemplateCount] = template;
    context->monsterTemplateCount++;
}

internal void CreateMonsterTemplates(RPGContext* context)
{
    #include "templates/monster_templates.inc"
    RPG_LOG("Loaded %d monster templates.\n", context->monsterTemplateCount);
}

internal void AddFaction(RPGContext* context, const char *name, i8 standing)
{
    context->factions[context->factionCount] = Faction_Create(name, standing);
    context->factionCount++;
}

/*
 * Context management
 */
void RPG_InitContext(RPGContext *context)
{
    RPG_LOG("Initializing context\n");
    memset(context, 0, sizeof(RPGContext));
    context->entityClasses[context->entityClassCount] = calloc(1, sizeof(EntityClass));
    EntityClass_InitFighter(context->entityClasses[context->entityClassCount]);
    context->entityClassCount++;
    CreateArmorTemplates(context);
    CreateWeaponTemplates(context);
    CreateMonsterTemplates(context);
    AddFaction(context, "Wildlife", RPG_MIN_STANDING);
    AddFaction(context, "Villagers", RPG_NEUTRAL_STANDING);
}

void RPG_ShutdownContext(RPGContext *context)
{
    RPG_LOG("Shutting down context\n");
    for(i32 i = 0; i < context->entityClassCount; ++i) {
        free(context->entityClasses[i]);
    }

    for(i32 i = 0; i < context->armorTemplateCount; ++i) {
        free(context->armorTemplates[i]);
    }

    for(i32 i = 0; i < context->weaponTemplateCount; ++i) {
        free(context->weaponTemplates[i]);
    }

    for(i32 i = 0; i < context->monsterTemplateCount; ++i) {
        free(context->monsterTemplates[i]);
    }

    for(i32 i = 0; i < context->factionCount; ++i) {
        Faction_Destroy(context->factions[i]);
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

/*
 * Armor template instancing functions
 */
ArmorTemplate *RPG_GetArmorTemplate(RPGContext *context, const char *templatename)
{
    for(i32 i = 0; i < context->armorTemplateCount; ++i) {
        ArmorTemplate *armorTemplate = context->armorTemplates[i];
        if(strcmp(templatename, armorTemplate->name) == 0) {
            return armorTemplate;
        }
    }
    return NULL;
}




/*
 * Weapon template instancing function
 */
WeaponTemplate *RPG_GetWeaponTemplate(RPGContext *context, const char *templatename)
{
    for(i32 i = 0; i < context->weaponTemplateCount; ++i) {
        WeaponTemplate *weaponTemplate = context->weaponTemplates[i];
        if(strcmp(templatename, weaponTemplate->name) == 0) {
            return weaponTemplate;
        }
    }
    return NULL;
}

/*
 * Monster template instancing functions
 */

MonsterTemplate *RPG_GetMonsterTemplate(RPGContext *context, const char *templatename)
{
    for(i32 i = 0; i < context->monsterTemplateCount; ++i) {
        MonsterTemplate *monsterTemplate = context->monsterTemplates[i];
        if(strcmp(templatename, monsterTemplate->name) == 0) {
            return monsterTemplate;
        }
    }
    return NULL;
}

Entity *RPG_CreateMonsterFromTemplate(RPGContext *context, MonsterTemplate *template, i32 level)
{
    Entity* entity = calloc(1, sizeof(Entity));
    Entity_Init(entity, ET_MONSTER, level, template->name, NULL);
    // load weapon if specified in template
    if(template->weaponTemplate) {
        WeaponTemplate* weaponTemplate = RPG_GetWeaponTemplate(context, template->weaponTemplate);
        if(weaponTemplate) {
            entity->weapon = Weapon_CreateFromTemplate(weaponTemplate);
        }
    }
    return entity;
}

void RPG_DestroyEntity(Entity *entity)
{
    switch(entity->type) {
        case ET_CHARACTER: {

            break;
        }
        case ET_MONSTER: {
            if(entity->weapon)
                Weapon_Destroy(entity->weapon);
            break;
        }
    }
    free(entity);
}