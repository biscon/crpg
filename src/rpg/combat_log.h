//
// Created by bison on 9/26/18.
//

#ifndef GAME_COMBAT_LOG_H
#define GAME_COMBAT_LOG_H

#include <store.h>
#include "../renderer/terminal.h"

#define COMBAT_LOG(_y, _x, ...) CombatLog_Print(&(_y), (_x), ##__VA_ARGS__)
//#define COMBAT_LOG(_y, _x, ...)

typedef struct CombatMessage    CombatMessage;
typedef struct CombatLog        CombatLog;

struct CombatMessage {
    u8 buffer[512];
};

struct CombatLog {
    Store messageStore;
};

void CombatLog_Create(CombatLog* log);
void CombatLog_Destroy(CombatLog* log);
void CombatLog_Print(CombatLog *log, const char *fmt, ...);
void CombatLog_Render(CombatLog* log, Terminal* term);

#endif
