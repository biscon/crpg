//
// Created by bison on 10/7/18.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <defs.h>
#include <cglm/call/vec4.h>

#define RENDERER_CMD_BUF_SIZE       8192
#define RENDERER_VERTEX_BUF_SIZE    65536
#define FLOATS_PER_VERTEX           8
#define VERTS_PER_QUAD              48

typedef struct RenderCmd RenderCmd;
typedef struct RenderCmdBuffer RenderCmdBuffer;
typedef struct RenderCmdClear RenderCmdClear;
typedef struct RenderCmdQuads RenderCmdQuads;
typedef struct RenderQuad RenderQuad;

typedef enum {
    RCMD_CLEAR,
    RCMD_QUAD,
    RCMD_TEX_QUAD
} RenderCmdType;

struct RenderCmd {
    RenderCmdType   type;
};

struct RenderCmdClear {
    RenderCmdType   type;
    vec4            color;
};

struct RenderCmdQuads {
    RenderCmdType   type;
    size_t          offset;
    size_t          vertexOffset;
    size_t          vertexCount;
};

struct RenderCmdBuffer {
    u8* commands;
    u8* quadVerts;
    u8* cmdOffset;
    u8* quadVertOffset;
};

struct RenderQuad {
    vec4    color;
    float   left;
    float   top;
    float   right;
    float   bottom;
};

void Render_CreateCmdBuffer(RenderCmdBuffer* buf);
void Render_DestroyCmdBuffer(RenderCmdBuffer* buf);
void Render_ClearCmdBuffer(RenderCmdBuffer* buf);
void Render_PushClearCmd(RenderCmdBuffer* buf, vec4 color);
void Render_PushQuadsCmd(RenderCmdBuffer* buf, RenderQuad *quads, size_t count);

#endif //GAME_RENDERER_H
