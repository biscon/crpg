//
// Created by bison on 11/4/18.
//

#ifndef GAME_COMBAT_MAP_H
#define GAME_COMBAT_MAP_H

#include <defs.h>
#include <list.h>
#include "../util/rex.h"

typedef struct Position Position;
typedef struct CombatMap CombatMap;
typedef struct CMTile CMTile;
typedef struct Path Path;


typedef enum {
                    CM_TT_NONE,
                    CM_TT_FLOOR,
                    CM_TT_WALL,
                    CM_TT_DOOR,
                    CM_TT_HOSTILE_START,
                    CM_TT_FRIENDLY_START,
} CMTileType;

struct Position {
    i32             x;
    i32             y;
};

struct Path {
    ListNode*       stepList; // List of Position structs
    i32             steps;
    i32             movesLeft;
};

struct CMTile {
    CMTileType      type;
    Position        position;
};

struct CombatMap {
    CMTile*         tiles;
    bool*           occupancyMap;
    u32             width;
    u32             height;
};

void        CombatMap_CreateFromTemplate(CombatMap* map, RexImage *template);
void        CombatMap_Destroy(CombatMap* map);
CMTile*     CombatMap_GetTileAt(CombatMap* map, u32 x, u32 y);
void        CombatMap_ReserveFriendlyStartPos(CombatMap* map, Position* pos);
void        CombatMap_ReserveHostileStartPos(CombatMap *map, Position *pos);
bool        CombatMap_IsOccupied(CombatMap* map, u32 x, u32 y);
void        CombatMap_SetOccupied(CombatMap* map, u32 x, u32 y, bool occupied);
bool        CombatMap_IsWalkable(CombatMap* map, u32 x, u32 y);
void        CombatMap_FindClosestAdjacentUnoccupiedPosition(CombatMap *map, const Position* target,
                                                            const Position* attacker, Position* result);
// pathfinding

Path*       CombatMap_CreatePath(CombatMap* map, Position *p1, Position *p2);
void        CombatMap_DestroyPath(Path* path);
#endif //GAME_COMBAT_MAP_H
