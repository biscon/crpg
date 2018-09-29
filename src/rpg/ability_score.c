//
// Created by bison on 9/23/18.
//

#include <stdio.h>
#include <string.h>
#include "ability_score.h"
#include "dice.h"

void AbilityScore_Roll(AbilityScore *score)
{
    score->STR = Dice_Roll("3d6");
    score->INT = Dice_Roll("3d6");
    score->WIS = Dice_Roll("3d6");
    score->DEX = Dice_Roll("3d6");
    score->CON = Dice_Roll("3d6");
    score->CHA = Dice_Roll("3d6");
}

void AbilityScore_Roll4D6DropLowest(AbilityScore *score)
{
    score->STR = Dice_Roll4D6DropLowest();
    score->INT = Dice_Roll4D6DropLowest();
    score->WIS = Dice_Roll4D6DropLowest();
    score->DEX = Dice_Roll4D6DropLowest();
    score->CON = Dice_Roll4D6DropLowest();
    score->CHA = Dice_Roll4D6DropLowest();
}

void AbilityScore_Print(AbilityScore *score, char *buffer, size_t bufsize)
{
    memset(buffer, 0, bufsize);
    snprintf(buffer, bufsize,
             "STR: %d\n"
             "INT: %d\n"
             "WIS: %d\n"
             "DEX: %d\n"
             "CON: %d\n"
             "CHA: %d\n",
             score->STR, score->INT, score->WIS, score->DEX, score->CON, score->CHA);
}

i32 AbilityScore_CalcBonusOrPenalty(i32 val) {
    i32 bonus = 0;
    if(val <= 1)
        bonus = -5;
    else if(val >=2 && val <=3)
        bonus = -4;
    else if(val >=4 && val <=5)
        bonus = -3;
    else if(val >= 6 && val <= 7)
        bonus = -2;
    else if(val >= 8 && val <= 9)
        bonus = -1;
    else if(val >= 10 && val <= 11)
        bonus = 0;
    else if(val >= 12 && val <= 13)
        bonus = 1;
    else if(val >= 14 && val <= 15)
        bonus = 2;
    else if(val >= 16 && val <= 17)
        bonus = 3;
    else if(val >= 18 && val <= 19)
        bonus = 4;
    else if(val >= 20 && val <= 21)
        bonus = 5;
    else if(val >= 22 && val <= 23)
        bonus = 6;
    else if(val >= 24 && val <= 25)
        bonus = 7;
    else if(val >= 26 && val <= 27)
        bonus = 8;
    else if(val >= 28 && val <= 29)
        bonus = 9;
    else if(val >= 30 && val <= 31)
        bonus = 10;

    return bonus;
}



