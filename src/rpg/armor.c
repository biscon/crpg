//
// Created by bison on 9/25/18.
//

#include <string.h>
#include <malloc.h>
#include "armor.h"
#include "rpg_defs.h"

void ArmorTemplate_Init(ArmorTemplate *armorTemplate, ArmorType type, const char *name, i32 ac,
                        i32 weight, i32 cost)
{
    memset(armorTemplate, 0, sizeof(ArmorTemplate));
    strncpy(armorTemplate->name, name, sizeof(armorTemplate->name));
    armorTemplate->armorType = type;
    armorTemplate->AC = ac;
    armorTemplate->weight = weight;
    armorTemplate->cost = cost;
}

