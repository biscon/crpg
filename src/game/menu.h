//
// Created by bison on 23-04-19.
//

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <defs.h>
#include <store.h>
#include <vector.h>
#include "../input/input.h"
#include "../renderer/font.h"
#include "../renderer/renderer.h"

typedef struct MenuItem     MenuItem;
typedef struct Menu         Menu;

typedef enum {
    MENU_TYPE_VERT,
    MENU_TYPE_HORIZ
} MenuType;


struct MenuItem {
    u32                     id;
    u8                      text[64];
    Vector                  children;
};

struct Menu {
    Vector                  items;
    MenuType                type;
    InputContext            inputContext;
    Font*                   font;
};

void Menu_Create(Menu* menu, MenuType type, Font* font);
void Menu_Destroy(Menu* menu);
void Menu_CreateItem(MenuItem* item);
void Menu_DestroyItem(MenuItem* item);
void Menu_AddItem(Menu* menu, MenuItem *item);
void Menu_AddChild(MenuItem *parent, MenuItem *child);
void Menu_Render(Menu* menu, RenderCmdBuffer *renderBuffer, double frameDelta);

#endif //GAME_MENU_H
