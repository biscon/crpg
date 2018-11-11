//
// Created by bison on 10/7/18.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <defs.h>
#include <cglm/call/vec4.h>
#include <store.h>
#include "pixel_buffer.h"
#include "font.h"
#include "texture_atlas.h"

#define RENDERER_CMD_BUF_SIZE           KILOBYTES(512)
#define RENDERER_VERTEX_BUF_SIZE        MEGABYTES(4)
#define FLOATS_PER_VERTEX               8
#define VERTS_PER_QUAD                  6

#define COLOR_TRANSPARENT               (vec4) {0.0f, 0.0f, 0.0f, 0.0f}
#define COLOR_WHITE                     (vec4) {1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_RED                       (vec4) {1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_MAGENTA                   (vec4) {1.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_YELLOW                    (vec4) {1.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE                      (vec4) {0.0f, 0.0f, 1.0f, 1.0f}


typedef struct RenderCmd                RenderCmd;
typedef struct RenderCmdBuffer          RenderCmdBuffer;
typedef struct RenderCmdClear           RenderCmdClear;
typedef struct RenderCmdQuads           RenderCmdQuads;
typedef struct Quad                     Quad;
typedef struct AtlasQuad                AtlasQuad;


typedef enum {
                                        RCMD_CLEAR,
                                        RCMD_QUAD,
                                        RCMD_TEX_QUAD,
                                        RCMD_TEX_QUAD_ATLAS,
} RenderCmdType;



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
        u8*                             quadVertsPtr;
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

// TODO push rotations and scale to the model matrices in the shaders allowing rects to spin'n'shit
void    Render_CreateCmdBuffer(RenderCmdBuffer* buf);
void    Render_DestroyCmdBuffer(RenderCmdBuffer* buf);
void    Render_ClearCmdBuffer(RenderCmdBuffer* buf);
void    Render_PushClearCmd(RenderCmdBuffer* buf, vec4 color);
void    Render_PushQuadsCmd(RenderCmdBuffer* buf, Quad *quads, size_t count);
void    Render_PushTexturedQuadsCmd(RenderCmdBuffer* buf, u32 texid,
                                    Quad *quads, size_t count);
void    Render_PushAtlasQuadsCmd(RenderCmdBuffer *buf, TextureAtlas *atlas,
                                 AtlasQuad *quads, size_t count);
void    Render_PushText(RenderCmdBuffer *buf, Font *font, float x, float y, vec4 color, const char* text);


#endif //GAME_RENDERER_H
