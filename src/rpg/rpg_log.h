//
// Created by bison on 9/26/18.
//

#ifndef GAME_RPG_LOG_H
#define GAME_RPG_LOG_H

#ifdef DEBUG
#define RPG_LOG(_x, ...) RPG_Log((_x), ##__VA_ARGS__)
#else
#define RPG_LOG(_x, ...)
#endif

void RPG_Log(const char *fmt, ...);

#endif //GAME_RPG_LOG_H
