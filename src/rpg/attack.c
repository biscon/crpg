//
// Created by bison on 9/29/18.
//

#include <string.h>
#include "attack.h"

void
Attack_Init(Attack *attack, AttackType type, const char *name, const char *dmgroll, bool ranged,
            i32 range, Weapon *weapon)
{
    memset(attack, 0 , sizeof(Attack));
    attack->type = type;
    strncpy(attack->name, name, sizeof(attack->name));
    strncpy(attack->damageRoll, dmgroll, sizeof(attack->damageRoll));
    attack->isRanged = ranged;
    attack->range = range;
    attack->weapon = weapon;
}

const char *Attack_GetTypeAsString(AttackType type) {
    switch(type)
    {
        case AT_MAIN_HAND: {
            return "mainhand";
        }
        case AT_OFF_HAND: {
            return "offhand";
        }
        case AT_TWO_HAND: {
            return "twohand";
        }
        case AT_MONSTER: {
            return "monster";
        }
    }
    return "Unknown";
}
