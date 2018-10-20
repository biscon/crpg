//
// Created by bison on 10/19/18.
//

#include <SDL_log.h>
#include "terminal.h"
#include "../util/string_util.h"

INTERNAL void SetPaletteEntry(Terminal* term, u32 palidx, const char* col_str)
{
    i32 r, g, b;
    sscanf(col_str, "#%02x%02x%02x", &r, &g, &b);

    term->palette[palidx].color[0] = (float) r/255.0f;
    term->palette[palidx].color[1] = (float) g/255.0f;
    term->palette[palidx].color[2] = (float) b/255.0f;
    term->palette[palidx].color[3] = 1.0f;
    //SDL_Log("Parsed string color %s into RGB = %d,%d,%d RGBA float = %.2f, %.2f, %.2f, %.2f", col_str, r, g, b, color[0], color[1], color[2], color[3]);
}

INTERNAL void SetANSIPalette(Terminal* term)
{
    SetPaletteEntry(term, 0, "#000000");      // BLACK
    SetPaletteEntry(term, 1, "#CC0000");      // RED
    SetPaletteEntry(term, 2, "#00CC00");      // GREEN
    SetPaletteEntry(term, 3, "#CCCC00");      // YELLOW
    SetPaletteEntry(term, 4, "#0000CC");      // BLUE
    SetPaletteEntry(term, 5, "#CC00CC");      // MAGENTA
    SetPaletteEntry(term, 6, "#00CCCC");      // CYAN
    SetPaletteEntry(term, 7, "#CCCCCC");      // WHITE
}

void Term_Create(Terminal *term, u32 w, u32 h, Font *font)
{
    memset(term, 0, sizeof(Terminal));
    term->width = w;
    term->height = h;
    term->font = font;
    term->charHeight = font->size;
    term->charWidth = Font_GetGlyphWidth(font);
    term->buffer = calloc(1, w * h * sizeof(CharCell));
    term->showCursor = true;
    SetANSIPalette(term);
    SDL_Log("Created Terminal %dx%d char size %dx%d px", w, h, term->charWidth, term->charHeight);
    Term_Clear(term);
    STORE_INIT(term->quadStore, sizeof(Quad));
    STORE_INIT(term->atlasQuadStore, sizeof(AtlasQuad));
}

void Term_Destroy(Terminal *term)
{
    free(term->buffer);
    STORE_DESTROY(term->quadStore);
    STORE_DESTROY(term->atlasQuadStore);
}

void Term_Clear(Terminal *term) {
    memset(term->buffer, 0, term->width * term->height * sizeof(CharCell));
    term->curFgColorIdx = TERM_COL_WHITE;
    term->curBgColorIdx = TERM_COL_BLACK;
}

void Term_Print(Terminal *term, i32 x, i32 y, const char *str)
{
    if(x < 0 || x >= term->width || y < 0 || y >= term->height)
        return;
    size_t len = strlen(str);
    for (int i=0; i < len;) {
        u32 cpLen;
        u32 cp = DecodeCodePoint(&cpLen, &str[i]);
        i += cpLen;

        CharCell* cc = (CharCell*) term->buffer;
        i32 index = y * term->width + x;
        cc[index].cp = cp;
        cc[index].bgColorIdx = term->curBgColorIdx;
        cc[index].fgColorIdx = term->curFgColorIdx;
        x += 1;
        if(x >= term->width) {
            y += 1;
            x = 0;
        }
        if(y >= term->height) {
            y = 0;
        }
    }
}

void Term_Render(Terminal *term, float x, float y, RenderCmdBuffer *buffer)
{
    STORE_CLEAR(term->quadStore);
    STORE_CLEAR(term->atlasQuadStore);
    float fx = x;
    float fy = y;
    for(i32 cy = 0; cy < term->height; ++cy) {
        for(i32 cx = 0; cx < term->width; ++cx) {
            CharCell* cc = (CharCell*) term->buffer;
            i32 index = cy * term->width + cx;
            Quad quad = {};
            memcpy(quad.color, term->palette[cc[index].bgColorIdx].color, sizeof(vec4));
            quad.left = x + (float) (cx * term->charWidth);
            quad.top = y + (float) (cy * term->charHeight);
            quad.right = quad.left + (float) term->charWidth;
            quad.bottom = quad.top + (float) term->charHeight;
            STORE_PUSHBACK(term->quadStore, &quad);

            Glyph* c = hashtable_find(&term->font->glyphTable, (HASHTABLE_U64) cc[index].cp);
            assert(c != NULL);

            float xp, yp, h, w;
            xp = quad.left + c->bearing[0];
            yp = quad.top  + term->charHeight - c->bearing[1];
            w = c->size[0];
            h = c->size[1];

            AtlasQuad atlasQuad = {
                    .left    = xp,
                    .top     = yp,
                    .right   = xp + w,
                    .bottom  = yp + h,
                    .atlasId = c->atlasId
            };
            memcpy(atlasQuad.color, term->palette[cc[index].fgColorIdx].color, sizeof(vec4));
            //atlasQuad.color[3] = 0.0f;
            STORE_PUSHBACK(term->atlasQuadStore, &atlasQuad);
            //fx += c->advance
        }
    }
    Render_PushQuadsCmd(buffer, term->quadStore.data, term->quadStore.noItems);
    Render_PushAtlasQuadsCmd(buffer, &term->font->atlas, term->atlasQuadStore.data, term->atlasQuadStore.noItems);
}

void Term_SetBGColor(Terminal *term, TermColor col) {
    term->curBgColorIdx = col;
}
