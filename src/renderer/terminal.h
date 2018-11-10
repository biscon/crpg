//
// Created by bison on 10/19/18.
//

#ifndef GAME_TERMINAL_H
#define GAME_TERMINAL_H

#include <defs.h>
#include "font.h"
#include "renderer.h"
#include "../util/rex.h"

typedef struct      Terminal            Terminal;
typedef struct      CharCell            CharCell;
typedef struct      TermPalEntry        TermPalEntry;

#define TERM_COL_BLACK          0x000000FF
#define TERM_COL_RED            0xFF0000FF
#define TERM_COL_GREEN          0x00FF00FF
#define TERM_COL_YELLOW         0xFFFF00FF
#define TERM_COL_BLUE           0x0000FFFF
#define TERM_COL_MAGENTA        0xFF00FFFF
#define TERM_COL_CYAN           0x00FFFFFF
#define TERM_COL_WHITE          0xFFFFFFFF


struct CharCell {
    u8              cp;
    u32             fgColor;
    u32             bgColor;
};

struct TermPalEntry {
    vec4            color;
};

struct Terminal {
    u32             width;
    u32             height;
    TextureAtlas*   fontAtlas;
    u32             charWidth;
    u32             charHeight;
    bool            showCursor;
    u32             padding;
    u32             curFgColor;
    u32             curBgColor;
    u8*             buffer;
    Store           quadStore;
    Store           atlasQuadStore;
};

void    Term_Create(Terminal* term, u32 w, u32 h, u32 cw, u32 ch, u32 pad, TextureAtlas* atlas);
void    Term_Destroy(Terminal* term);
void    Term_Render(Terminal* term, float x, float y, RenderCmdBuffer *buffer);
void    Term_Clear(Terminal* term);
void    Term_Print(Terminal* term, i32 x, i32 y, const char* str);
void    Term_SetBGColor(Terminal* term, u32 col);
void    Term_SetFGColor(Terminal* term, u32 col);
void    Term_PrintRexImage(Terminal* term, RexImage* image, i32 x, i32 y);

static inline
void    Term_SetXY(Terminal* term, u8 ch, i32 x, i32 y, u32 bgcol, u32 fgcol) {
    CharCell* cc = (CharCell*) term->buffer;
    i32 index = y * term->width + x;
    cc[index].cp = ch;
    cc[index].bgColor = bgcol;
    cc[index].fgColor = fgcol;
}

static inline
void    Term_SetXYNoBG(Terminal* term, u8 ch, i32 x, i32 y, u32 fgcol) {
    CharCell* cc = (CharCell*) term->buffer;
    i32 index = y * term->width + x;
    cc[index].cp = ch;
    cc[index].fgColor = fgcol;
}

#endif //GAME_TERMINAL_H
