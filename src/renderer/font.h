//
// Created by bison on 10/14/18.
//
#ifndef GAME_FONT_H
#define GAME_FONT_H

#include <defs.h>
#include <hashtable.h>
#include "texture_atlas.h"

typedef struct Font Font;
typedef i32 ivec2[2];
typedef struct Glyph Glyph;

struct Glyph {
    u32 atlasId;
    u32 advance;
    i32 size[2];
    i32 bearing[2];
};

typedef struct FT_FaceRec_*  FT_Face;

struct Font {
    FT_Face face;
    u32 size;
    hashtable_t glyphTable;
    TextureAtlas atlas;
};


bool Font_Init();
void Font_Shutdown();
bool Font_Create(Font *font, const char *path, u32 size);
void Font_Destroy(Font* font);

#endif //GAME_FONT_H
