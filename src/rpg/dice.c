//
// Created by bison on 9/23/18.
//

#include "dice.h"
#include "rpg_log.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void Dice_Init()
{
    time_t seed = time(NULL);
    srand(seed);
}

i32 Dice_Roll(const char *string) {
    i32 no_rolls = 0;
    i32 pips = 1;
    i32 adds = 0;
    i32 sum = 0;
    sscanf(string, "%dd%d+%d", &no_rolls, &pips, &adds);

    for(i32 i = 0; i < no_rolls; ++i)
    {
        i32 roll = (rand() % pips)+1;
        sum += roll;
    }
    sum += adds;
    //RPG_LOG("Rolling dice (no_rolls = %d, pips = %d, adds = %d) = %d\n", no_rolls, pips, adds, sum);
    RPG_LOG("Rolling dice %s = %d\n", string, sum);
    return sum;
}

i32 Dice_Roll4D6DropLowest() {
    i32 a = Dice_Roll("1d6");
    i32 b = Dice_Roll("1d6");
    i32 c = Dice_Roll("1d6");
    i32 d = Dice_Roll("1d6");
    i32 sum = a + b + c + d;
    int min_ab, min_cd, min;
    min_ab = a < b ? a : b;
    min_cd = c < d ? c : d;
    min = min_ab < min_cd ? min_ab : min_cd;
    return sum-min;
}
