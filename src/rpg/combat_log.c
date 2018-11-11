//
// Created by bison on 9/26/18.
//

#include "combat_log.h"

#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <malloc.h>

void CombatLog_Create(CombatLog *log) {
    memset(log, 0, sizeof(CombatLog));
    STORE_INIT(log->messageStore, sizeof(CombatMessage));

    CombatLog_Print(log,"Line 1");
    CombatLog_Print(log,"Det havde du ellers ikke sidst vi snakkede sammen. Men det er måske længe siden du har fået noget på den dumme?");
    CombatLog_Print(log,"Line 5");
    CombatLog_Print(log,"Line 6");
    CombatLog_Print(log,"Line 7");
    CombatLog_Print(log,"Line 8");
}

void CombatLog_Destroy(CombatLog *log) {
    STORE_DESTROY(log->messageStore);
}

void CombatLog_Print(CombatLog *log, const char *fmt, ...)
{
    CombatMessage msg;
    va_list args;
    va_start(args, fmt);
    vsnprintf((char *) msg.buffer, sizeof(msg.buffer), fmt, args);
    perror((char *) msg.buffer);
    va_end(args);
    STORE_PUSHBACK(log->messageStore, &msg);
    if(log->messageStore.noItems > 10) {
        STORE_REMOVE_AT(log->messageStore, 0);
    }
}

INTERNAL
i32 CalcActualLines(CombatLog* log, i32 width)
{
    if(log->messageStore.noItems == 0)
        return 0;
    i32 total_lines = 0;
    for(u32 i = 0; i < log->messageStore.noItems; ++i) {
        CombatMessage *msg = STORE_GET_AT(log->messageStore, i);
        assert(msg != NULL);
        size_t len = strlen((char *) msg->buffer);
        i32 lines = (i32) len / width;
        if(len % width > 0)
            lines += 1;
        total_lines += lines;
    }
    return total_lines;
}

void CombatLog_Render(CombatLog* log, Terminal* term)
{
    //Term_SetBGColor(term, 0x003040FF);
    Term_Clear(term);
    //Term_DrawBGRect(term, 0, 0, term->width-1, term->height-1, 0x003040FF);
    u32 fgcol = 0xD9A300FF;
    // top
    Term_DrawLine(term, 0, 0, term->width-1, 0, fgcol, 196);
    // bottom
    Term_DrawLine(term, 0, term->height-1, term->width-1, term->height-1, fgcol, 196);
    // left
    Term_DrawLine(term, 0, 0, 0, term->height-1, fgcol, 179);
    // right
    Term_DrawLine(term, term->width-1, 0, term->width-1, term->height-1, fgcol, 179);

    // corners
    Term_SetXYNoBG(term, 218, 0,0, 0xD9A300FF);
    Term_SetXYNoBG(term, 191, term->width-1, 0, 0xD9A300FF);
    Term_SetXYNoBG(term, 217, term->width-1, term->height-1, 0xD9A300FF);
    Term_SetXYNoBG(term, 192, 0, term->height-1, 0xD9A300FF);

    Term_Print(term, 2, 0, " LOG ");
    Term_SetXYNoBG(term, 180, 1, 0, 0xD9A300FF);
    Term_SetXYNoBG(term, 195, 7, 0, 0xD9A300FF);
    i32 width = term->width-2;
    i32 height = term->height-2;
    i32 x = 1;
    i32 y = 1;
    if(log->messageStore.noItems == 0)
        return;

    i32 actual_lines = CalcActualLines(log, width);
    // draw from top and down
    if(actual_lines <= height) {
        for(u32 i = 0; i < log->messageStore.noItems; ++i) {
            CombatMessage *msg = STORE_GET_AT(log->messageStore, i);
            assert(msg != NULL);
            size_t len = strlen((char *) msg->buffer);
            for(u32 j = 0; j < len; ++j) {
                Term_SetXYNoBG(term, msg->buffer[j], x, y, 0x999999FF);
                x++;
                if(x > width-1) {
                    x = 1;
                    y++;
                }
            }
            x = 1;
            y++;
        }
    } else {    // draw from bottom and up
        y = height+1;
        for(i32 i = log->messageStore.noItems-1; i >= 0; i--) {
            CombatMessage *msg = STORE_GET_AT(log->messageStore, (u32) i);
            assert(msg != NULL);
            size_t len = strlen((char *) msg->buffer);

            i32 lines = (i32) len / width;
            if (len % width > 0)
                lines += 1;
            y -= lines;


            if(y < -10)
                return;
            i32 tmp_y = y;
            for(u32 j = 0; j < len; ++j) {
                if(tmp_y >= 1 && tmp_y <= height)
                    Term_SetXYNoBG(term, msg->buffer[j], x, tmp_y, 0x999999FF);
                x++;
                if(x > width-1) {
                    x = 1;
                    tmp_y++;
                }
            }
            x = 1;
        }
    }
}