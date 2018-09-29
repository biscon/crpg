//
// Created by bison on 9/26/18.
//

#include <string.h>
#include <malloc.h>
#include "weapon.h"
#include "rpg_defs.h"

void
WeaponTemplate_Init(WeaponTemplate *weaponTemplate, const char *name, i32 cost, WeaponSize size,
                    i32 weight, const char *damage, i32 range, WeaponCategory cat) {

    memset(weaponTemplate, 0, sizeof(WeaponTemplate));
    strncpy(weaponTemplate->name, name, sizeof(weaponTemplate->name));
    strncpy(weaponTemplate->damage, damage, sizeof(weaponTemplate->damage));
    weaponTemplate->cost = cost;
    weaponTemplate->size = size;
    weaponTemplate->category = cat;
    weaponTemplate->weight = weight;
    weaponTemplate->range = range;
}

Weapon *Weapon_CreateFromTemplate(WeaponTemplate *template)
{
    Weapon* armor = calloc(1, sizeof(Weapon));
    armor->condition = RPG_CONDITION_MAX;
    armor->template = template;
    return armor;
}

void Weapon_Destroy(Weapon *weapon)
{
    free(weapon);
}
