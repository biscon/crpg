//
// Created by bison on 9/26/18.
//

#include <string.h>
#include <malloc.h>
#include "monster.h"

void MonsterTemplate_Init(MonsterTemplate *template, const char *name, i32 ac, const char *attacks,
                          const char *dmgdices, i32 xp, const char *weapontpl,
                          const char *factiontpl, const char *aicls, const char *grammarcls)
{
    memset(template, 0, sizeof(MonsterTemplate));
    if(name)
        strncpy(template->name, name, sizeof(template->name));
    template->AC = ac;
    if(attacks)
        strncpy(template->attacks, attacks, sizeof(template->attacks));
    if(dmgdices)
        strncpy(template->damageDices, dmgdices, sizeof(template->damageDices));
    template->XP = xp;
    if(weapontpl)
        strncpy(template->weaponTemplate, weapontpl, sizeof(template->weaponTemplate));
    if(factiontpl)
        strncpy(template->factionTemplate, factiontpl, sizeof(template->factionTemplate));
    if(aicls)
        strncpy(template->aiClass, aicls, sizeof(template->aiClass));
    if(grammarcls)
        strncpy(template->grammarClass, grammarcls, sizeof(template->grammarClass));
}
