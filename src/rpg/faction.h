//
// Created by bison on 9/29/18.
//

#ifndef GAME_FACTION_H
#define GAME_FACTION_H

#include <defs.h>

typedef struct {
    char    name[128];
    i8      standing;       // -127 = Hates player, 0 = Neutral, 127 = Wanna have players babies
} Faction;

Faction*    Faction_Create(const char *name, i8 standing);
void        Faction_Destroy(Faction* faction);
#endif //GAME_FACTION_H
