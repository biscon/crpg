//
// Created by bison on 10/14/18.
//

#include "font.h"

#include <ft2build.h>
#include <defs.h>
#include <glad.h>
#include <SDL_log.h>
#include FT_FREETYPE_H

//#define HASHTABLE_IMPLEMENTATION
#define HASHTABLE_U32 u32
#define HASHTABLE_U64 u64
#include "hashtable.h"
#include "renderer.h"

#define EN 0x01
#define EN_START 0x00
#define EN_END   0x80
#define KR 0x02
#define KR_START 0xAC00
#define KR_END   0xD7A3


typedef i32 ivec2[2];
typedef struct Glyph Glyph;

struct Glyph {
    u32 atlasId;
    GLuint advance;
    i32 size[2];
    i32 bearing[2];
};

struct Font {
    FT_Face face;
    u8 size;
    hashtable_t* glyphTable;
    TextureAtlas atlas;
};

FT_Library library;

bool Font_Init() {
    i32 error;
    error = FT_Init_FreeType(&library);
    return error ? true : false; // 1 if error
}

void Font_Shutdown() {
    FT_Done_FreeType(library);
}

INTERNAL
void LoadGlyphs(Font *font, u32 cpStart, u32 cpEnd) {
    for (u32 cp = cpStart; cp <= cpEnd; cp += 1) {
        if (FT_Load_Char(font->face, cp, FT_LOAD_RENDER)) {
            SDL_Log("Failed to load glyph for codepoint: 0x%x\n", cp);
            continue;
        }

        PixelBuffer pb;
        u32 w = font->face->glyph->bitmap.width;
        u32 h = font->face->glyph->bitmap.rows;
        PixelBuffer_Create(&pb, w, h, PBF_GREYSCALE);
        memcpy(pb.pixels, font->face->glyph->bitmap.buffer, (size_t) w*h);
        u32 atlas_id = TextureAtlas_AddImage(&font->atlas, &pb);
        PixelBuffer_Destroy(&pb);
        Glyph ch = {
                atlas_id,
                (GLuint) (font->face->glyph->advance.x >> 6),
        };
        ch.size[0] = w;
        ch.size[1] = h;
        ch.bearing[0] = font->face->glyph->bitmap_left;
        ch.bearing[1] = font->face->glyph->bitmap_top;
        hashtable_insert(font->glyphTable, (HASHTABLE_U64) cp, &ch);
    }
}

bool Font_Create(Font *font, const char *path, u8 size)
{
    TextureAtlas_Create(&font->atlas, 4096, 4096, PBF_GREYSCALE);
    FT_Face face;
    if (FT_New_Face(library, path, 0, &face))
        return false;
    if (FT_Set_Pixel_Sizes(face, 0, size))
        return false;
    font->face = face;
    font->size = size;
    LoadGlyphs(font, EN_START, EN_END);
    TextureAtlas_PackAndUpload(&font->atlas);
    //LoadGlyphs(font, KR_START, KR_END);
    return true;
}

void Font_Destroy(Font* font)
{
    if(font->face)
        FT_Done_Face(font->face);
    hashtable_term(font->glyphTable);
    TextureAtlas_Destroy(&font->atlas);
}