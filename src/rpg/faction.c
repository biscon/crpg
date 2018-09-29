//
// Created by bison on 9/29/18.
//

#include <string.h>
#include <malloc.h>
#include "faction.h"

Faction *Faction_Create(const char *name, i8 standing) {
    Faction* faction = calloc(1, sizeof(Faction));
    strncpy(faction->name, name, sizeof(faction->name));
    faction->standing = standing;
    return faction;
}

void Faction_Destroy(Faction *faction) {
    free(faction);
}
