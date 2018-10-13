//
// Created by bison on 10/13/18.
//

#include <SDL_log.h>
#include "pixel_buffer.h"
#include "../util/lodepng.h"

void PixelBuffer_Create(PixelBuffer *pb, u32 width, u32 height)
{
    pb->width = width;
    pb->height = height;
    pb->pixels = calloc(1, width * height * sizeof(u32));
}

bool PixelBuffer_CreateFromPNG(PixelBuffer *pb, const char *filename)
{
    unsigned error;
    error = lodepng_decode32_file((u8**) &pb->pixels, &pb->width, &pb->height, filename);
    if(error) {
        SDL_Log("error %u: %s\n", error, lodepng_error_text(error));
        return false;
    }
    return true;
}

void PixelBuffer_Destroy(PixelBuffer* pb)
{
    if(pb->pixels)
        free(pb->pixels);
}

void PixelBuffer_SimpleBlit(PixelBuffer *src, u32 *src_rect,
                            PixelBuffer *dst, u32 *dst_pos)
{
    u32 height = src_rect[3];
    u32 *src_ptr = src->pixels;
    src_ptr += (src_rect[1] * src->width) + src_rect[0];
    u32 *dst_ptr = dst->pixels;
    dst_ptr += (dst_pos[1] * dst->width) + dst_pos[0];
    u32 src_inc = src->width;
    u32 dst_inc = dst->width;
    u32 src_width_bytes = src_rect[2] * sizeof(u32);
    //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
    for(u32 y = 0; y < height; ++y)
    {
        memcpy(dst_ptr, src_ptr, src_width_bytes);
        src_ptr += src_inc;
        dst_ptr += dst_inc;
    }
}
