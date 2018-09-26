//
// Created by bison on 9/24/18.
//

#include <string.h>
#include <malloc.h>
#include "rpg.h"
#include "dice.h"
#include "entity_class.h"

/*
 * Helper functions for creating armor templates
 */
internal void AddArmorTemplate(RPGContext* context, ArmorType type, const char *name, i32 ac, i32 weight, i32 cost)
{
    ArmorTemplate *template = calloc(1, sizeof(ArmorTemplate));
    ArmorTemplate_Init(template, type, name, ac, weight, cost);
    context->armorTemplates[context->armorTemplateCount] = template;
    context->armorTemplateCount++;
}

internal void CreateArmorTemplates(RPGContext* context)
{
    #include "templates/armor_templates.inc"
}

/*
 * Helper functions for creating weapon templates
 */
internal void AddWeaponTemplate(RPGContext* context, const char* name, i32 cost,
                                WeaponSize size, i32 weight, const char* damage, i32 range,
                                WeaponCategory cat)
{
    WeaponTemplate *template = calloc(1, sizeof(WeaponTemplate));
    WeaponTemplate_Init(template, name, cost, size, weight, damage, range, cat);
    context->weaponTemplates[context->weaponTemplateCount] = template;
    context->weaponTemplateCount++;
}

internal void CreateWeaponTemplates(RPGContext* context)
{
    #include "templates/weapon_templates.inc"
}

/*
 * Helper functions for creating monster templates
 */
internal void AddMonsterTemplate(RPGContext* context, const char *name, i32 ac,
                                 const char *attacks, const char *dmgdices, i32 xp,
                                 const char *weapontpl, const char *factiontpl, const char *aicls,
                                 const char *grammarcls)
{
    MonsterTemplate *template = calloc(1, sizeof(MonsterTemplate));
    MonsterTemplate_Init(template, name, ac, attacks, dmgdices, xp, weapontpl, factiontpl, aicls, grammarcls);
    context->monsterTemplates[context->monsterTemplateCount] = template;
    context->monsterTemplateCount++;
}

internal void CreateMonsterTemplates(RPGContext* context)
{
    #include "templates/monster_templates.inc"
}



/*
 * Context management
 */
void RPG_InitContext(RPGContext *context)
{
    memset(context, 0, sizeof(RPGContext));
    context->entityClasses[context->entityClassCount] = calloc(1, sizeof(EntityClass));
    EntityClass_InitFighter(context->entityClasses[context->entityClassCount]);
    context->entityClassCount++;
    CreateArmorTemplates(context);
    CreateWeaponTemplates(context);
    CreateMonsterTemplates(context);
}

void RPG_ShutdownContext(RPGContext *context)
{
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
