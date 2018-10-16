//
// Created by bison on 9/23/18.
//
#ifndef GAME_STRING_UTIL_H
#define GAME_STRING_UTIL_H

#include <defs.h>

u32 DecodeCodePoint(u32 *cpLen, const char *str);
u32 EncodeCodePoint(u8 *buffer, u32 cp);

char * TrimString(char *str);

#endif //GAME_STRING_UTIL_H
