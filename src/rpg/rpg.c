//
// Created by bison on 9/24/18.
//

#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "rpg.h"
#include "dice.h"
#include "entity_class.h"
#include "monster.h"
#include "rpg_defs.h"
#include "rpg_log.h"
#include "entity.h"
#include "attack.h"
#include "armor.h"

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
        EntityClass_Free(context->entityClasses[i]);
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

// TODO implement name prefixes based on stat eg low int = Retarded Werewolf, low cha = Ugly Coyote etc
Entity *RPG_CreateMonsterFromTemplate(RPGContext *context, MonsterTemplate *template, i32 level)
{
    Entity* entity = calloc(1, sizeof(Entity));
    Entity_Init(entity, ET_MONSTER, level, template->name, NULL, template);
    return entity;
}

void RPG_DestroyEntity(Entity *entity)
{
    switch(entity->type) {
        case ET_CHARACTER: {
            if(entity->mainWeapon)
                Weapon_Destroy(entity->mainWeapon);
            if(entity->offWeapon)
                Weapon_Destroy(entity->offWeapon);
            if(entity->shield)
                Armor_Destroy(entity->shield);
            if(entity->armor)
                Armor_Destroy(entity->armor);
            break;
        }
        case ET_MONSTER: {
            break;
        }
    }
    free(entity);
}

/*
 * Character instancing functions
 */
Entity *
RPG_CreateCharacterEntity(RPGContext *context, EntityClass* entityClass, const char *name, i32 level)
{
    Entity* entity = calloc(1, sizeof(Entity));
    assert(entityClass != NULL);
    assert(name != NULL);
    Entity_Init(entity, ET_CHARACTER, level, name, entityClass, NULL);
    return entity;
}

void RPG_LogEntity(Entity *entity) {
    RPG_LOG("=====================================================================\n");
    RPG_LOG("Name:\t\t%s\n", entity->name);
    RPG_LOG("Level:\t\t%d\n", entity->level);
    RPG_LOG("HP:\t\t\t%d\n", entity->HP);
    RPG_LOG("MaxHP:\t\t%d\n", entity->maxHP);
    RPG_LOG("XP:\t\t\t%d\n", entity->XP);
    RPG_LOG("Money:\t\t%d\n", entity->money);
    RPG_LOG("AC:\t\t\t%d\n", Entity_GetAC(entity));
    RPG_LOG("Class:\t\t");
    if(entity->type == ET_CHARACTER) {
        assert(entity->entityClass != NULL);
        RPG_LOG("%s\n", entity->entityClass->name);
        RPG_LOG("Bonuses:\tCON = %d, DEX = %d, STR = %d\n", Entity_GetCONBonus(entity),
        Entity_GetDEXBonus(entity), Entity_GetSTRBonus(entity));
        if(entity->armor != NULL)
        {
            RPG_LOG("Armor:\t\t%s\n", entity->armor->template->name);
        }
        if(entity->shield != NULL)
        {
            RPG_LOG("Shield:\t\t%s\n", entity->shield->template->name);
        }
    }
    else if(entity->type == ET_MONSTER) {
        RPG_LOG("Monster\n");
    }
    RPG_LOG("Attack bonus:\t%d\n\n", Entity_GetAttackBonus(entity));

    AbilityScore* score = &entity->abilityScore;
    RPG_LOG("Ability Scores:\n");
    RPG_LOG( "\tSTR: \t\t%d\n"
             "\tINT: \t\t%d\n"
             "\tWIS: \t\t%d\n"
             "\tDEX: \t\t%d\n"
             "\tCON: \t\t%d\n"
             "\tCHA: \t\t%d\n",
             score->STR, score->INT, score->WIS, score->DEX, score->CON, score->CHA);

    if(entity->attackCount > 0) {
        RPG_LOG("\nAttacks:\n");
        for (i32 i = 0; i < entity->attackCount; ++i) {
            Attack* a = entity->attacks[i];
            RPG_LOG("\t%s: \t%s (%s)\n", Attack_GetTypeAsString(a->type), a->name, a->damageRoll);
        }
    }
    RPG_LOG("=====================================================================\n");
}

