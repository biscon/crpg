//
// Created by bison on 9/26/18.
//

#ifndef GAME_MONSTER_H
#define GAME_MONSTER_H
#include <defs.h>
#include "weapon.h"
#include "grammar.h"

typedef struct {
    char                name[RPG_STR_SIZE_MEDIUM];
    i32                 AC;
    char                attacks[RPG_STR_SIZE_LARGE];
    char                damageDices[RPG_STR_SIZE_LARGE];
    i32                 XP;
    char                weaponTemplate[RPG_STR_SIZE_MEDIUM];
    char                factionTemplate[RPG_STR_SIZE_MEDIUM];
    char                aiClass[RPG_STR_SIZE_MEDIUM];
    GrammarType         grammarType;
} MonsterTemplate;

void                    MonsterTemplate_Init(MonsterTemplate* template, const char *name, i32 ac,
                          const char *attacks, const char *dmgdices, i32 xp, const char *weapontpl,
                          const char *factiontpl, const char *aicls, GrammarType grammarType);

#endif //GAME_MONSTER_H
