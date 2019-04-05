//
// Created by bison on 4/3/19.
//

#ifndef GAME_MAPPED_INPUT_H
#define GAME_MAPPED_INPUT_H

#include <defs.h>

typedef u32 MappedInputId;

enum {
    INPUT_ACTION_UP,
    INPUT_ACTION_DOWN,
    INPUT_ACTION_LEFT,
    INPUT_ACTION_RIGHT,
    INPUT_ACTION_SELECT,
    INPUT_ACTION_BACK,
};

enum {
    INPUT_STATE_FORWARD,
    INPUT_STATE_BACK,
    INPUT_STATE_LEFT,
    INPUT_STATE_RIGHT,
};

#endif //GAME_MAPPED_INPUT_H
