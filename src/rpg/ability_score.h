//
// Created by bison on 9/23/18.
//

#ifndef GAME_ABILITY_SCORE_H
#define GAME_ABILITY_SCORE_H

#include <defs.h>

typedef struct
{
        i32 STR;
        i32 INT;
        i32 WIS;
        i32 DEX;
        i32 CON;
        i32 CHA;
} AbilityScore;

void    AbilityScore_Roll(AbilityScore *score);
void    AbilityScore_Roll4D6DropLowest(AbilityScore *score);
void    AbilityScore_Print(AbilityScore *score, char *buffer, size_t bufsize);
i32     AbilityScore_CalcBonusOrPenalty(i32 val);

#endif //GAME_ABILITY_SCORE_H
