//
// Created by bison on 10/13/18.
//

#ifndef GAME_PIXEL_BUFFER_H
#define GAME_PIXEL_BUFFER_H

#include <defs.h>

typedef struct              PixelBuffer PixelBuffer;

typedef enum {
                            PBF_RGBA,
                            PBF_GREYSCALE
} PixelBufferFormat;

struct PixelBuffer {
    PixelBufferFormat       format;
    u32                     width;
    u32                     height;
    void*                   pixels;
};


void    PixelBuffer_Create(PixelBuffer* pb, u32 width, u32 height, PixelBufferFormat format);
bool    PixelBuffer_CreateFromPNG(PixelBuffer* pb, const char* filename);
void    PixelBuffer_Destroy(PixelBuffer* pb);
void    PixelBuffer_SimpleBlit(PixelBuffer* src, uvec4 src_rect,
                            PixelBuffer* dst, uvec2 dst_pos);

#endif //GAME_PIXEL_BUFFER_H
