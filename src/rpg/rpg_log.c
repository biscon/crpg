//
// Created by bison on 9/26/18.
//

#include "rpg_log.h"

#include <stdio.h>
#include <zconf.h>
#include <string.h>

void RPG_Log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buffer[512] = {0};
    //strncpy(buffer, "RPG: ", sizeof(buffer));
    strncat(buffer, fmt, sizeof(buffer));
    vprintf(buffer, args);
    va_end(args);
}
