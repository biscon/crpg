//
// Created by bison on 9/29/18.
//

#ifndef GAME_ATTACK_H
#define GAME_ATTACK_H

#include <defs.h>
#include "weapon.h"

typedef enum {AT_MAIN_HAND, AT_OFF_HAND, AT_TWO_HAND, AT_MONSTER} AttackType;

typedef struct {
    AttackType  type;
    char        name[RPG_STR_SIZE_MEDIUM];
    char        damageRoll[RPG_STR_SIZE_MEDIUM];
    bool        isRanged;
    i32         range;
    Weapon      *weapon;
} Attack;

void            Attack_Init(Attack* attack, AttackType type, const char *name, const char *dmgroll,
                    bool ranged, i32 range, Weapon* weapon);

const char*     Attack_GetTypeAsString(AttackType type);

#endif //GAME_ATTACK_H
