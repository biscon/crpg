//
// Created by bison on 9/26/18.
//

#ifndef GAME_WEAPON_H
#define GAME_WEAPON_H

#include <defs.h>

typedef enum        {WS_SMALL, WS_MEDIUM, WS_LARGE}     WeaponSize;
typedef enum        {WC_SHARP, WC_BLUNT, WC_GUN}        WeaponCategory;

typedef struct {
    WeaponCategory  category;
    WeaponSize      size;
    char            name[64];
    char            damage[16];
    i32             cost;
    i32             weight;
    i32             range;
} WeaponTemplate;

typedef struct {
    WeaponTemplate* template;
    i8              condition;  // Broken AF -127 <-> 127 Mint condition
    // here you would add stuff affecting a particular piece of armor, like enchantments
} Weapon;

void                WeaponTemplate_Init(WeaponTemplate *weaponTemplate, const char* name, i32 cost,
                         WeaponSize size, i32 weight, const char* damage, i32 range,
                         WeaponCategory cat);

Weapon*             Weapon_CreateFromTemplate(WeaponTemplate *template);
void                Weapon_Destroy(Weapon *weapon);

#endif //GAME_WEAPON_H
