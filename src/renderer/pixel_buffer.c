//
// Created by bison on 10/13/18.
//

#include <SDL_log.h>
#include "pixel_buffer.h"
#include "../util/lodepng.h"

void PixelBuffer_Create(PixelBuffer *pb, u32 width, u32 height, PixelBufferFormat format)
{
    pb->width = width;
    pb->height = height;
    pb->format = format;
    if(format == PBF_RGBA)
        pb->pixels = calloc(1, width * height * sizeof(u32));
    if(format == PBF_GREYSCALE)
        pb->pixels = calloc(1, width * height);
}

bool PixelBuffer_CreateFromPNG(PixelBuffer *pb, const char *filename)
{
    unsigned error;
    error = lodepng_decode32_file((u8**) &pb->pixels, &pb->width, &pb->height, filename);
    pb->format = PBF_RGBA;
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


INTERNAL
void RGBA_SimpleBlit(PixelBuffer *src, u32 *src_rect,
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

INTERNAL
void GREYSCALE_SimpleBlit(PixelBuffer *src, u32 *src_rect,
                          PixelBuffer *dst, u32 *dst_pos)
{
    u32 height = src_rect[3];
    u8 *src_ptr = src->pixels;
    src_ptr += (src_rect[1] * src->width) + src_rect[0];
    u8 *dst_ptr = dst->pixels;
    dst_ptr += (dst_pos[1] * dst->width) + dst_pos[0];
    u32 src_inc = src->width;
    u32 dst_inc = dst->width;
    u32 src_width_bytes = src_rect[2];
    //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
    for(u32 y = 0; y < height; ++y)
    {
        memcpy(dst_ptr, src_ptr, src_width_bytes);
        src_ptr += src_inc;
        dst_ptr += dst_inc;
    }
}


void PixelBuffer_SimpleBlit(PixelBuffer *src, u32 *src_rect,
                            PixelBuffer *dst, u32 *dst_pos)
{
    if(src->format != dst->format) {
        SDL_Log("Can't blit between different pixel formats");
        return;
    }
    switch(src->format) {
        case PBF_RGBA: {
            RGBA_SimpleBlit(src, src_rect, dst, dst_pos);
            break;
        }
        case PBF_GREYSCALE: {
            GREYSCALE_SimpleBlit(src, src_rect, dst, dst_pos);
            break;
        }
    }
}
