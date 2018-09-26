//
// Created by bison on 9/26/18.
//

#ifndef GAME_MONSTER_H
#define GAME_MONSTER_H
#include <defs.h>

typedef struct {
    char            name[64];
    i32             AC;
    char            attacks[128];
    char            damageDices[128];
    i32             XP;
    char            weaponTemplate[64];
    char            factionTemplate[64];
    char            aiClass[64];
    char            grammarClass[64];
} MonsterTemplate;

void MonsterTemplate_Init(MonsterTemplate* template, const char *name, i32 ac,
                          const char *attacks, const char *dmgdices, i32 xp, const char *weapontpl,
                          const char *factiontpl, const char *aicls, const char *grammarcls);

#endif //GAME_MONSTER_H
