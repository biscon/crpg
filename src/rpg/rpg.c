//
// Created by bison on 9/24/18.
//

#include <string.h>
#include <malloc.h>
#include "rpg.h"
#include "dice.h"
#include "entity_class.h"

internal void AddArmorTemplate(RPGContext* context, ArmorType type, const char *name, i32 ac, i32 weight, i32 cost)
{
    ArmorTemplate *template = calloc(1, sizeof(ArmorTemplate));
    ArmorTemplate_Init(template, type, name, ac, weight, cost);
    context->armorTemplates[context->armorTemplateCount] = template;
    context->armorTemplateCount++;
}

internal void CreateArmorTemplates(RPGContext* context)
{
    AddArmorTemplate(context, AT_CHEST, "Leather Armor", 13, 15, 20);
    AddArmorTemplate(context, AT_CHEST, "Chain Mail", 15, 40, 60);
    AddArmorTemplate(context, AT_CHEST, "Plate Mail", 17, 50, 300);
    AddArmorTemplate(context, AT_SHIELD, "Shield", 1, 5, 7);
}

void RPG_InitContext(RPGContext *context)
{
    memset(context, 0, sizeof(RPGContext));
    context->entityClasses[context->entityClassCount] = calloc(1, sizeof(EntityClass));
    EntityClass_InitFighter(context->entityClasses[context->entityClassCount]);
    context->entityClassCount++;
    CreateArmorTemplates(context);
}

void RPG_ShutdownContext(RPGContext *context)
{
    for(i32 i = 0; i < context->entityClassCount; ++i) {
        free(context->entityClasses[i]);
    }

    for(i32 i = 0; i < context->armorTemplateCount; ++i) {
        free(context->armorTemplates[i]);
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
