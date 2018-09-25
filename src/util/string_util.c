//
// Created by bison on 9/23/18.
//

#include "string_util.h"

u32 DecodeCodePoint(u32 *cpLen, const char *str) {
    static const u32 FIRST_LEN[] = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1
    };

    static const u8 MASK[] = {
            0xFF, 0xFF, 0x1F, 0xF, 0x7
    };

    u8 b0 = str[0];
    i32 l = FIRST_LEN[b0];
    i32 val = (i32)(b0 & MASK[l]);

    for (i32 i = 1; i < l; i += 1) {
        val = (val << 6) | (i32)(str[i] & 0x3f);
    }

    if (cpLen)
        *cpLen = l;

    return val;
}

u32 EncodeCodePoint(u8 *buffer, const u32 cp) {
    if (cp <= 0x7F) {
        buffer[0] = cp;
        return 1;
    }

    if (cp <= 0x7FF) {
        buffer[0] = 0xC0 | (cp >> 6);
        buffer[1] = 0x80 | (cp & 0x3F);
        return 2;
    }

    if (cp <= 0xFFFF) {
        buffer[0] = 0xE0 | (cp >> 12);
        buffer[1] = 0x80 | ((cp >> 6) & 0x3F);
        buffer[2] = 0x80 | (cp & 0x3F);
        return 3;
    }

    if (cp <= 0x10FFFF) {
        buffer[0] = 0xF0 | (cp >> 18);
        buffer[1] = 0x80 | ((cp >> 12) & 0x3F);
        buffer[2] = 0x80 | ((cp >> 6) & 0x3F);
        buffer[3] = 0x80 | (cp & 0x3F);
        return 4;
    }

    return 0;
}
