//
// Created by bison on 23-04-19.
//

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <defs.h>
#include <store.h>
#include <vector.h>

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
};

#endif //GAME_MENU_H
