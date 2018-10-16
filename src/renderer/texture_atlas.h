//
// Created by bison on 10/16/18.
//

#ifndef GAME_TEXTURE_ATLAS_H
#define GAME_TEXTURE_ATLAS_H

#include <defs.h>
#include <store.h>
#include "pixel_buffer.h"

typedef struct TextureAtlas         TextureAtlas;
typedef struct TextureAtlasEntry    TextureAtlasEntry;
typedef struct FloatRect            FloatRect;

struct FloatRect {
    float                           left;
    float                           top;
    float                           right;
    float                           bottom;
};

struct TextureAtlasEntry {
    u32                             id;
    PixelBuffer                     pixelBuffer;
    FloatRect                       uvRect;
};

typedef struct hashtable_t          hashtable_t;
struct TextureAtlas {
    hashtable_t*                    entryTable;
    Store                           rectStore;
    i32                             noRects;
    i32                             width;
    i32                             height;
    u32                             nextEntryId;
    u32                             textureId;
    bool                            isUploaded;
    PixelBufferFormat               format;
};

void    TextureAtlas_Create(TextureAtlas *atlas, i32 width, i32 height, PixelBufferFormat format);
void    TextureAtlas_Destroy(TextureAtlas *atlas);
u32     TextureAtlas_AddImageFromPNG(TextureAtlas *atlas, const char *filename);
u32     TextureAtlas_AddImage(TextureAtlas *atlas, PixelBuffer* src_buf);
void    TextureAtlas_PackAndUpload(TextureAtlas *atlas);
void    TextureAtlas_SetUVRect(TextureAtlas* atlas, u32 id, FloatRect* rect);

#endif //GAME_TEXTURE_ATLAS_H
