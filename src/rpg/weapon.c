//
// Created by bison on 9/26/18.
//

#include <string.h>
#include "weapon.h"

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
