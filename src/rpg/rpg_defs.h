//
// Created by bison on 9/26/18.
//

#ifndef GAME_RPG_DEFS_H
#define GAME_RPG_DEFS_H

#define RPG_MAX_CLASSES                 128
#define RPG_MAX_ARMOR_TEMPLATES         128
#define RPG_MAX_WEAPON_TEMPLATES        128
#define RPG_MAX_MONSTER_TEMPLATES       128
#define RPG_MAX_FACTIONS                64
#define RPG_MAX_ENTITY_ATTACKS          32

#define RPG_CONDITION_MAX               127
#define RPG_CONDITION_MIN               (-127)

#define RPG_MIN_STANDING                (-127)
#define RPG_NEUTRAL_STANDING            0
#define RPG_MAX_STANDING                127

#define RPG_BASE_AC                     10
#define RPG_COMBAT_EVENT_STACK_SIZE     1024
#define RPG_DICE_STR_SIZE               16
#define RPG_STR_SIZE_SMALL              32
#define RPG_STR_SIZE_MEDIUM             64
#define RPG_STR_SIZE_LARGE              128
#define RPG_STR_SIZE_XLARGE             256

#define RPG_CRIT_MODIFIER               1.5

#define RPG_GRID_W                      9
#define RPG_GRID_H                      4
#endif //GAME_RPG_DEFS_H
