//
// Created by bison on 11/4/18.
//

#include <memory.h>
#include <assert.h>
#include <malloc.h>
#include "combat_map.h"
#include "rpg_log.h"


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
    return (y * map->width) + x;
}

CMTile *CombatMap_GetTileAt(CombatMap *map, u32 x, u32 y)
{
    u32 idx = PosToIdx(map, x, y);
    CMTile *tile = &map->tiles[idx];
    assert(tile != NULL);
    return tile;
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

