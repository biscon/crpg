//
// Created by bison on 23-04-19.
//

#include "menu.h"

void Menu_Create(Menu *menu, MenuType type, Font* font) {
    VectorAlloc(&menu->items);
    menu->type = type;
    menu->font = font;
    Input_CreateContext(&menu->inputContext);
}

void Menu_Destroy(Menu *menu) {
    Input_DestroyContext(&menu->inputContext);
    for(i32 i = 0; i < menu->items.total; ++i) {
        Menu_DestroyItem(VectorGet(&menu->items, i));
    }
    VectorFree(&menu->items);
}

void Menu_CreateItem(MenuItem *item) {
    VectorAlloc(&item->children);
}

void Menu_DestroyItem(MenuItem* item) {
    VectorFree(&item->children);
}

void Menu_AddItem(Menu *menu, MenuItem *item) {
    VectorAdd(&menu->items, item);
}

void Menu_AddChild(MenuItem *parent, MenuItem *child) {
    VectorAdd(&parent->children, child);
}

void Menu_Render(Menu *menu, RenderCmdBuffer *renderBuffer, double frameDelta) {
    i32 y = 100;
    for(i32 i = 0; i < menu->items.total; ++i) {
        MenuItem* item = VectorGet(&menu->items, i);
        Render_PushText(renderBuffer, menu->font, 100, y, COLOR_WHITE, (char*) item->text);
        y += menu->font->size + (menu->font->size/4);
    }
}
