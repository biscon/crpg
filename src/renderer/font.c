//
// Created by bison on 10/14/18.
//

#include "font.h"
#include <ft2build.h>
//#include <glad.h>
#include <SDL_log.h>
#include <assert.h>
#include FT_FREETYPE_H

//#define HASHTABLE_IMPLEMENTATION

#define EN 0x01
#define EN_START 0x00
#define EN_END   0xFF
#define KR 0x02
#define KR_START 0xAC00
#define KR_END   0xD7A3


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
                (u32) (font->face->glyph->advance.x >> 6),
        };
        ch.size[0] = w;
        ch.size[1] = h;
        ch.bearing[0] = font->face->glyph->bitmap_left;
        ch.bearing[1] = font->face->glyph->bitmap_top;
        hashtable_insert(&font->glyphTable, (HASHTABLE_U64) cp, &ch);
    }
}

bool Font_Create(Font *font, const char *path, u32 size)
{
    TextureAtlas_Create(&font->atlas, 4096, 4096, PBF_GREYSCALE);
    FT_Face face;
    if (FT_New_Face(library, path, 0, &face))
        return false;
    if (FT_Set_Pixel_Sizes(face, 0, size))
        return false;
    font->face = face;
    font->size = size;
    font->monoWidth = (u32) font->face->max_advance_width >> 6;
    font->monoHeight = (u32) font->face->max_advance_height >> 6;
    SDL_Log("Mono dimensions = %d,%d", font->monoWidth, font->monoHeight);
    hashtable_init(&font->glyphTable, sizeof(Glyph), 256, NULL);
    LoadGlyphs(font, EN_START, EN_END);
    TextureAtlas_PackAndUpload(&font->atlas);
    //LoadGlyphs(font, KR_START, KR_END);
    return true;
}

void Font_Destroy(Font* font)
{
    if(font->face)
        FT_Done_Face(font->face);
    hashtable_term(&font->glyphTable);
    TextureAtlas_Destroy(&font->atlas);
}

u32 Font_GetGlyphWidth(Font *font) {
    const Glyph* glyph = (Glyph*) hashtable_items(&font->glyphTable);
    assert(glyph != NULL);
    return (u32) glyph->advance;
}
