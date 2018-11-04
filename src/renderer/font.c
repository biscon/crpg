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

/*
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
 */

INTERNAL
void LoadGlyphs(Font *font, u32 cpStart, u32 cpEnd) {
    for (u32 cp = cpStart; cp <= cpEnd; cp += 1) {

        FT_UInt index = FT_Get_Char_Index(font->face, cp);
        FT_Error err = FT_Load_Glyph(font->face, index, FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
        if (err) {
            SDL_Log("Failed to load glyph for codepoint: 0x%x\n", cp);
            continue;
        }
        //FT_Glyph_Metrics metrics = font->face->glyph->metrics;

        PixelBuffer pb;
        u32 w = font->face->glyph->bitmap.width;
        u32 h = font->face->glyph->bitmap.rows;

        //SDL_Log("Glyph w,h %d,%d", w, h);

        //PixelBuffer_Create(&pb, w+2, h+2, PBF_GREYSCALE);
        PixelBuffer_Create(&pb, w, h, PBF_GREYSCALE);
        //u8* pixels = (u8*) pb.pixels + pb.width + 1;
        u8* pixels = (u8*) pb.pixels;
        u8* src_buf = font->face->glyph->bitmap.buffer;

        for(i32 y = 0; y < h; ++y) {
            for(i32 x = 0; x < w; ++x) {
                *pixels = src_buf[(y*w)+x];
                pixels++;
            }
            //pixels += 2;
        }

        //w+=2;
        //h+=2;

        //memcpy(pb.pixels, dst_buf, (size_t) w*h);
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
    TextureAtlas_Create(&font->atlas, 1024, 1024, PBF_GREYSCALE);
    FT_Face face;
    if (FT_New_Face(library, path, 0, &face))
        return false;
    if (FT_Set_Pixel_Sizes(face, 0, size))
        return false;
    font->face = face;
    font->size = size;

    SDL_Log("Creating font size %d", font->size);

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
