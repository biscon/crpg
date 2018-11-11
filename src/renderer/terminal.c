//
// Created by bison on 10/19/18.
//

#include <SDL_log.h>
#include "terminal.h"
#include "../util/string_util.h"

/*
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
 */

void Term_Create(Terminal *term, u32 w, u32 h, u32 cw, u32 ch, u32 pad, TextureAtlas* atlas)
{
    memset(term, 0, sizeof(Terminal));
    term->width = w;
    term->height = h;
    term->fontAtlas = atlas;
    //term->charHeight = font->size;
    //SDL_Log("font pady = %.2f", pady);
    term->charHeight = ch;
    term->charWidth = cw;
    term->padding = pad;
    term->buffer = calloc(1, w * h * sizeof(CharCell));
    term->showCursor = true;
    //SetANSIPalette(term);
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
    term->curFgColor = TERM_COL_WHITE;
    term->curBgColor = TERM_COL_BLACK;
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
        cc[index].cp = (u8) cp;
        //cc[index].bgColor = term->curBgColor;
        cc[index].fgColor = term->curFgColor;
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

INTERNAL void ColorToRGBA(u32 color, u8* r, u8* g, u8* b, u8* a)
{
    *r = (color & 0xFF000000) >> 24;
    *g = (color & 0x00FF0000) >> 16;
    *b = (color & 0x0000FF00) >> 8;
    *a = (color & 0x000000FF);
}

INTERNAL void RGBAToColor(u8 r, u8 g, u8 b, u8 a, u32* col) {
    u32 color = 0;
    color |= r << 24;
    color |= g << 16;
    color |= b << 8;
    color |= a;
    *col = color;
}

/*
void Term_SetXY(Terminal* term, u8 ch, i32 x, i32 y, u32 bgcol, u32 fgcol) {
    CharCell* cc = (CharCell*) term->buffer;
    i32 index = y * term->width + x;
    cc[index].cp = ch;
    cc[index].bgColor = bgcol;
    cc[index].fgColor = fgcol;
}
*/

void Term_Render(Terminal *term, float x, float y, RenderCmdBuffer *buffer)
{
    STORE_CLEAR(term->quadStore);
    STORE_CLEAR(term->atlasQuadStore);

    u32 cellW = term->charWidth + (2 * term->padding);
    u32 cellH = term->charHeight + (2 * term->padding);
    u8 r,g,b,a;

    for(i32 cy = 0; cy < term->height; ++cy) {
        for(i32 cx = 0; cx < term->width; ++cx) {
            // background
            CharCell* cc = (CharCell*) term->buffer;
            i32 index = cy * term->width + cx;

            Quad quad = {};
            ColorToRGBA(cc[index].bgColor, &r, &g, &b, &a);
            quad.color[0] = r/255.0f;
            quad.color[1] = g/255.0f;
            quad.color[2] = b/255.0f;
            quad.color[3] = a/255.0f;
            quad.left = x + (float) (cx * cellW);
            quad.top = y + (float) (cy * cellH);
            quad.right = quad.left + (float) cellW;
            quad.bottom = quad.top + (float) cellH;
            STORE_PUSHBACK(term->quadStore, &quad);

            AtlasQuad atlasQuad = {
                    .left    = quad.left + term->padding,
                    .top     = quad.top + term->padding,
                    .right   = atlasQuad.left + term->charWidth,
                    .bottom  = atlasQuad.top + term->charHeight,
                    .atlasId = cc[index].cp
            };
            //SDL_Log("Char %d", cc[index].cp);
            ColorToRGBA(cc[index].fgColor, &r, &g, &b, &a);
            atlasQuad.color[0] = r/255.0f;
            atlasQuad.color[1] = g/255.0f;
            atlasQuad.color[2] = b/255.0f;
            atlasQuad.color[3] = a/255.0f;
            STORE_PUSHBACK(term->atlasQuadStore, &atlasQuad);
            //fx += c->advance
        }
    }
    Render_PushQuadsCmd(buffer, term->quadStore.data, term->quadStore.noItems);
    Render_PushAtlasQuadsCmd(buffer, term->fontAtlas, term->atlasQuadStore.data, term->atlasQuadStore.noItems);
}

void Term_SetBGColor(Terminal *term, u32 col)
{
    term->curBgColor = col;
}

void Term_SetFGColor(Terminal *term, u32 col)
{
    term->curFgColor = col;
}


void Term_PrintRexImage(Terminal* term, RexImage *image, i32 x, i32 y)
{
    for(i32 l = 0; l < image->noLayers; ++l) {
        RexLayer* layer = STORE_GET_AT(image->layerStore, l);
        assert(layer != NULL);
        for(i32 cx = 0; cx < layer->width; ++cx) {
            for(i32 cy = 0; cy < layer->height; ++cy) {
                RexTile *tile = &layer->tiles[(cx * layer->height) + cy];
                if(tile->back_red == 255 && tile->back_green == 0 && tile->back_blue == 255) {
                    continue;
                }

                u8 ch = tile->character;
                u32 fgcol, bgcol;
                RGBAToColor(tile->back_red, tile->back_green, tile->back_blue, 255, &bgcol);
                RGBAToColor(tile->fore_red, tile->fore_green, tile->fore_blue, 255, &fgcol);
                //SDL_Log("cx, cy = %d,%d", cx,cy);
                Term_SetXY(term, ch, x + cx, y + cy, bgcol, fgcol);
            }
        }
    }

}

/*
 * Good old bresenham
 */
void Term_DrawLine(Terminal* term, i32 x0, i32 y0, i32 x1, i32 y1, u32 col, u8 ch) {
    i32 dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    i32 dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    i32 err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        Term_SetXYNoBG(term, ch, x0, y0, col);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void Term_DrawBGRect(Terminal* term, i32 x0, i32 y0, i32 x1, i32 y1, u32 col)
{
    for(i32 x = x0; x <= x1; ++x) {
        for(i32 y = y0; y <= y1; ++y) {
            CharCell* cc = (CharCell*) term->buffer;
            i32 index = y * term->width + x;
            cc[index].bgColor = col;
        }
    }
}
