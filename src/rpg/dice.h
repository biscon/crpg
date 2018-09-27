//
// Created by bison on 9/23/18.
//
#ifndef GAME_DICE_H
#define GAME_DICE_H

#include <defs.h>

void        Dice_Init();
i32         Dice_Roll(const char *string);
i32         Dice_Roll4D6DropLowest();

#endif //GAME_DICE_H
