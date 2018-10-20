//
// Created by bison on 10/19/18.
//

#ifndef GAME_TERMINAL_H
#define GAME_TERMINAL_H

#include <defs.h>
#include "font.h"
#include "renderer.h"

typedef struct      Terminal            Terminal;
typedef struct      CharCell            CharCell;
typedef struct      TermPalEntry        TermPalEntry;

typedef enum {
    TERM_COL_BLACK,
    TERM_COL_RED,
    TERM_COL_GREEN,
    TERM_COL_YELLOW,
    TERM_COL_BLUE,
    TERM_COL_MAGENTA,
    TERM_COL_CYAN,
    TERM_COL_WHITE
} TermColor;

struct CharCell {
    u32             cp;
    u32             fgColorIdx;
    u32             bgColorIdx;
};

struct TermPalEntry {
    vec4            color;
};

struct Terminal {
    u32             width;
    u32             height;
    Font*           font;
    u32             charWidth;
    u32             charHeight;
    bool            showCursor;
    u32             curFgColorIdx;
    u32             curBgColorIdx;
    u8*             buffer;
    TermPalEntry    palette[256];
    Store           quadStore;
    Store           atlasQuadStore;
};

void    Term_Create(Terminal* term, u32 w, u32 h, Font *font);
void    Term_Destroy(Terminal* term);
void    Term_Render(Terminal* term, float x, float y, RenderCmdBuffer *buffer);
void    Term_Clear(Terminal* term);
void    Term_Print(Terminal* term, i32 x, i32 y, const char* str);
void    Term_SetBGColor(Terminal* term, TermColor col);

#endif //GAME_TERMINAL_H
