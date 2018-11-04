//
// Created by bison on 11/3/18.
//

#ifndef GAME_REX_H
#define GAME_REX_H

#include <defs.h>
#include <store.h>

typedef struct RexTile RexTile;
typedef struct RexLayer RexLayer;
typedef struct RexImage RexImage;

struct  RexTile {
    u8 character;
    u8 padding[3]; //Do not use. Accounts for the unused final 3 bytes in .xp tile characters (v 1.03).
    u8 fore_red;
    u8 fore_green;
    u8 fore_blue;
    u8 back_red;
    u8 back_green;
    u8 back_blue;
};


struct RexLayer {
    RexTile *tiles;
    u32 width;
    u32 height;
};

struct RexImage {
    Store   layerStore;
    i32     noLayers;
    i32     version;
};

void Rex_CreateFromFile(RexImage* image, const char *filename);
void Rex_Destroy(RexImage* image);

#endif //GAME_REX_H
