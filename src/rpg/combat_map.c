//
// Created by bison on 11/4/18.
//

#include <memory.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "combat_map.h"
#include "rpg_log.h"
#include "AStar.h"

void CombatMap_CreateFromTemplate(CombatMap *map, RexImage *template)
{
    memset(map, 0, sizeof(CombatMap));


    RexLayer* layer = STORE_GET_AT(template->layerStore, 0);
    assert(layer != NULL);
    map->width = layer->width;
    map->height = layer->height;
    map->tiles = calloc(1, layer->width * layer->height * sizeof(CMTile));
    map->occupancyMap = calloc(1, layer->width * layer->height * sizeof(bool));

    CMTile* tiles_ptr = map->tiles;

    for(u32 cx = 0; cx < layer->width; ++cx) {
        for(u32 cy = 0; cy < layer->height; ++cy) {
            RexTile *tile = &layer->tiles[(cx * layer->height) + cy];
            CMTile maptile = {.position.x = cx, .position.y = cy};
            u8 ch = tile->character;
            switch(ch) {
                case '.':
                    maptile.type = CM_TT_FLOOR;
                    break;
                case '#':
                    maptile.type = CM_TT_WALL;
                    break;
                case '+':
                    maptile.type = CM_TT_DOOR;
                    break;
                case 2:
                    maptile.type = CM_TT_FRIENDLY_START;
                    break;
                case 'E':
                    maptile.type = CM_TT_HOSTILE_START;
                    break;
                default:
                    maptile.type = CM_TT_NONE;
            }
            //u32 fgcol, bgcol;
            *tiles_ptr = maptile;
            tiles_ptr++;
        }
    }
}

void CombatMap_Destroy(CombatMap *map)
{
    if(map->tiles)
        free(map->tiles);
    if(map->occupancyMap)
        free(map->occupancyMap);
}

INTERNAL inline
u32 PosToIdx(CombatMap* map, u32 x, u32 y)
{
    //return (y * map->width) + x;
    return (x * map->height) + y;
}

CMTile *CombatMap_GetTileAt(CombatMap *map, u32 x, u32 y)
{
    u32 idx = PosToIdx(map, x, y);
    CMTile *tile = &map->tiles[idx];
    assert(tile != NULL);
    return tile;
}

void CombatMap_Render(CombatMap* map, Terminal* term)
{
    for(i32 x = 0; x < map->width; ++x) {
        for(i32 y = 0; y < map->height; ++y) {
            CMTile* tile = CombatMap_GetTileAt(map, x, y);
            switch(tile->type) {
                case CM_TT_FRIENDLY_START:
                case CM_TT_HOSTILE_START:
                case CM_TT_FLOOR: {
                    Term_SetXY(term, '.', x, y, MAP_COL_BACK, MAP_COL_FLOOR);
                    break;
                }
                case CM_TT_WALL: {
                    Term_SetXY(term, '#', x, y, MAP_COL_BACK, MAP_COL_WALL);
                    break;
                }
                case CM_TT_DOOR: {
                    Term_SetXY(term, '+', x, y, MAP_COL_BACK, MAP_COL_DOOR);
                    break;
                }
                default: {
                    Term_SetXY(term, 0, x, y, MAP_COL_BACK, TERM_COL_WHITE);
                    break;
                }
            }
        }
    }
}

INTERNAL
void ReserveStartPos(CombatMap* map, Position* pos, CMTileType type) {
    for(u32 y = 0; y < map->height; ++y) {
        for(u32 x = 0; x < map->width; ++x) {
            u32 idx = PosToIdx(map, x,y);
            bool occupied = map->occupancyMap[idx];
            if(!occupied && map->tiles[idx].type == type) {
                pos->x = x;
                pos->y = y;
                map->occupancyMap[idx] = true;
                return;
            }
        }
    }
    RPG_LOG("Could not place entity on map, out of spots\n");
    pos->x = -1;
    pos->y = -1;
}

void CombatMap_ReserveFriendlyStartPos(CombatMap *map, Position *pos)
{
    ReserveStartPos(map, pos, CM_TT_FRIENDLY_START);
}

void CombatMap_ReserveHostileStartPos(CombatMap *map, Position *pos)
{
    ReserveStartPos(map, pos, CM_TT_HOSTILE_START);
}

bool CombatMap_IsOccupied(CombatMap *map, u32 x, u32 y)
{
    return map->occupancyMap[PosToIdx(map, x, y)];
}

void CombatMap_SetOccupied(CombatMap* map, u32 x, u32 y, bool occupied) {
    map->occupancyMap[PosToIdx(map, x, y)] = occupied;
}

bool CombatMap_IsWalkable(CombatMap* map, u32 x, u32 y)
{
    CMTile* tile = &map->tiles[PosToIdx(map, x, y)];
    assert(tile != NULL);
    if( tile->type == CM_TT_FLOOR ||
        tile->type == CM_TT_HOSTILE_START ||
        tile->type == CM_TT_FRIENDLY_START) {
        return true;
    }
    return false;
}

inline INTERNAL i32 GetDistance(const Position* c1, const Position* c2)
{
    return abs((i32) sqrt((c2->x-c1->x)*(c2->x-c1->x) +
                          (c2->y-c1->y)*(c2->y-c1->y)));
}

// take the sqrt of me to get the actual distance ;) however that is much slower :D
inline INTERNAL i32 GetDistanceSquared(const Position* c1, const Position* c2)
{
    return (c2->x-c1->x)*(c2->x-c1->x) +
           (c2->y-c1->y)*(c2->y-c1->y);
}

// TODO this only finds free tiles immediately around an enemy, to support moving partially
// towards enemies in a turn it would be smarter to find an incomplete path with a-star
void CombatMap_FindClosestAdjacentUnoccupiedPosition(CombatMap *map, const Position* target, const Position* attacker, Position* result)
{
    const Position *pos = target;
    i32 gx, gy;
    // go through each sides, if occupied find distance and choose the largest
    // this is faster than doing jamming them in a container and sorting :)
    i32 mindist = INT32_MAX;
    Position minpos = {-1, -1};

    if(pos->x >= 1) {   // left
        gx = pos->x-1;
        gy = pos->y;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {   // check that the tile isn't occupied
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, attacker);
            //RPG_LOG("left %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(pos->x < map->width) {    // right
        gx = pos->x+1;
        gy = pos->y;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, attacker);
            //RPG_LOG("right %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(pos->y >= 1) {   // top
        gx = pos->x;
        gy = pos->y-1;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, attacker);
            //RPG_LOG("top %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(pos->y < map->height) {   // bottom
        gx = pos->x;
        gy = pos->y+1;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, attacker);
            //RPG_LOG("bottom %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(mindist != INT32_MAX) {
        result->x = minpos.x;
        result->y = minpos.y;
    } else {
        result->x = INT32_MAX;
        result->y = INT32_MAX;
    }
}

/*
 * Pathfinding
 * If introducing diagonals remember to set edge coast to 1.4f
 */
INTERNAL void GetNodeNeighbors(ASNeighborList neighbors, void *node, void *context)
{
    Position* pos = (Position*) node;
    CombatMap* map = (CombatMap*) context;
    i32 gx, gy;
    //RPG_LOG("GetNodeNeighbors query node at %d,%d\n", pos->x, pos->y);
    if(pos->x >= 1) {   // add left
        //RPG_LOG("Adding left %d,%d\n", pos->x-1, pos->y);
        gx = pos->x-1;
        gy = pos->y;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position p = {.x = gx, .y = gy};
            ASNeighborListAdd(neighbors, &p, 1.0f);
        }
    }
    if(pos->x < map->width) {    // add right
        //RPG_LOG("Adding right %d,%d\n", pos->x+1, pos->y);
        gx = pos->x + 1;
        gy = pos->y;
        if (!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position p = {.x = gx, .y = gy};
            ASNeighborListAdd(neighbors, &p, 1.0f);
        }
    }
    if(pos->y >= 1) {   // add top
        //RPG_LOG("Adding top %d,%d\n", pos->x, pos->y-1);
        gx = pos->x;
        gy = pos->y-1;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position p = {.x = gx, .y = gy};
            ASNeighborListAdd(neighbors, &p, 1.0f);
        }
    }
    if(pos->y < map->height) {   // add bottom
        //RPG_LOG("Adding bottom %d,%d\n", pos->x, pos->y+1);
        gx = pos->x;
        gy = pos->y+1;
        if(!CombatMap_IsOccupied(map, gx, gy) && CombatMap_IsWalkable(map, gx, gy)) {
            Position p = {.x = gx, .y = gy};
            ASNeighborListAdd(neighbors, &p, 1.0f);
        }
    }
}

INTERNAL float GetPathCostHeuristic(void *fromNode, void *toNode, void *context)
{
    Position* from = (Position*) fromNode;
    Position* to = (Position*) toNode;
    float res = (float) fabs(sqrt(GetDistanceSquared(from, to)));
    //RPG_LOG("Returning distance heuristic (%d,%d --> %d,%d) %f\n", from->x, from->y, to->x, to->y, res);
    return res;
}


Path* CombatMap_CreatePath(CombatMap* map, Position *p1, Position *p2)
{
    ASPathNodeSource source;
    memset(&source, 0, sizeof(ASPathNodeSource));
    source.nodeSize = sizeof(Position*);
    source.nodeNeighbors = GetNodeNeighbors;
    source.pathCostHeuristic = GetPathCostHeuristic;
    void *from = p1;
    void *to = p2;
    ASPath path = ASPathCreate(&source, map, from, to);
    if(path) {
        RPG_LOG("A path was found between %d,%d and %d,%d (steps: %d)\n", p1->x, p1->y, p2->x, p2->y, ASPathGetCount(path));
        Path *result = calloc(1, sizeof(Path));
        result->steps = (i32) ASPathGetCount(path);
        for(i32 i = result->steps-1; i > 0 ; i--) {
            Position* oldnode = (Position*) ASPathGetNode(path, (size_t) i);
            Position* newnode = calloc(1, sizeof(Position));
            newnode->x = oldnode->x;
            newnode->y = oldnode->y;
            LIST_PUSH(result->stepList, newnode);
            RPG_LOG("\t\tStep %d:\t%d,%d\n", i, newnode->x, newnode->y);
        }
        ASPathDestroy(path);
        return result;
    } else {
        RPG_LOG("Pathfinding failed for reasons unknown :/\n");
    }
    return NULL;
}

void CombatMap_DestroyPath(Path* path)
{
    ListNode* cur = path->stepList;
    while(cur != NULL) {
        free(cur->data);
        cur = cur->next;
    }
    LIST_DESTROY(path->stepList);
    free(path);
}

