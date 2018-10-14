//
// Created by bison on 10/7/18.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <defs.h>
#include <cglm/call/vec4.h>
#include <store.h>
#include "pixel_buffer.h"

#define RENDERER_CMD_BUF_SIZE       KILOBYTES(32)
#define RENDERER_VERTEX_BUF_SIZE    KILOBYTES(256)
#define FLOATS_PER_VERTEX           8
#define VERTS_PER_QUAD              48

typedef struct RenderCmd            RenderCmd;
typedef struct RenderCmdBuffer      RenderCmdBuffer;
typedef struct RenderCmdClear       RenderCmdClear;
typedef struct RenderCmdQuads       RenderCmdQuads;
typedef struct Quad                 Quad;
typedef struct AtlasQuad            AtlasQuad;
typedef struct TextureAtlas         TextureAtlas;
typedef struct TextureAtlasEntry    TextureAtlasEntry;
typedef struct FloatRect            FloatRect;

typedef enum {
                                    RCMD_CLEAR,
                                    RCMD_QUAD,
                                    RCMD_TEX_QUAD,
                                    RCMD_TEX_QUAD_ATLAS,
} RenderCmdType;

struct FloatRect {
    float                           left;
    float                           top;
    float                           right;
    float                           bottom;
};

struct TextureAtlasEntry {
    u32                             id;
    char                            filename[256];
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
};

struct RenderCmd {
    RenderCmdType                   type;
};

struct RenderCmdClear {
    RenderCmdType                   type;
    vec4                            color;
};

struct RenderCmdQuads {
    RenderCmdType                   type;
    size_t                          offset;
    size_t                          vertexOffset;
    size_t                          vertexCount;
    u32                             texId;
    TextureAtlas*                   atlas;
};

struct RenderCmdBuffer {
    u8*                             commands;
    u8*                             quadVerts;
    u8*                             cmdOffset;
    u8*                             quadVertOffset;
};

struct Quad {
    vec4                            color;
    float                           left;
    float                           top;
    float                           right;
    float                           bottom;
};

struct AtlasQuad {
    vec4                            color;
    u32                             atlasId;
    float                           left;
    float                           top;
    float                           right;
    float                           bottom;
};

void    Render_CreateCmdBuffer(RenderCmdBuffer* buf);
void    Render_DestroyCmdBuffer(RenderCmdBuffer* buf);
void    Render_ClearCmdBuffer(RenderCmdBuffer* buf);
void    Render_PushClearCmd(RenderCmdBuffer* buf, vec4 color);
void    Render_PushQuadsCmd(RenderCmdBuffer* buf, Quad *quads, size_t count);
void    Render_PushTexturedQuadsCmd(RenderCmdBuffer* buf, u32 texid,
                                    Quad *quads, size_t count);
void    Render_PushAtlasQuadsCmd(RenderCmdBuffer *buf, TextureAtlas *atlas,
                                 AtlasQuad *quads, size_t count);

void    TextureAtlas_Create(TextureAtlas *atlas, i32 width, i32 height);
void    TextureAtlas_Destroy(TextureAtlas *atlas);
u32     TextureAtlas_AddImage(TextureAtlas *atlas, const char *filename);
void    TextureAtlas_PackAndUpload(TextureAtlas *atlas);
void    TextureAtlas_SetUVRect(TextureAtlas* atlas, u32 id, FloatRect* rect);

#endif //GAME_RENDERER_H
