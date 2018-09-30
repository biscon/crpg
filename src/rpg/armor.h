//
// Created by bison on 9/25/18.
//

#ifndef GAME_ARMOR_H
#define GAME_ARMOR_H

#include <defs.h>
#include "rpg_defs.h"

typedef enum        {AT_CHEST, AT_SHIELD} ArmorType;

typedef struct {
    ArmorType       armorType;
    char            name[RPG_STR_SIZE_MEDIUM];
    i32             cost;
    i32             weight;
    i32             AC;
} ArmorTemplate;

typedef struct {
    ArmorTemplate*  template;
    i8              condition;  // Broken AF -127 <-> 127 Mint condition
    // here you would add stuff affecting a particular piece of armor, like enchantments
} Armor;

void                ArmorTemplate_Init(ArmorTemplate *armorTemplate, ArmorType type,
                                       const char* name, i32 ac, i32 weight, i32 cost);
Armor*              Armor_CreateFromTemplate(ArmorTemplate *template);
void                Armor_Destroy(Armor *armor);
#endif //GAME_ARMOR_H
