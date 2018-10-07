//
// Created by bison on 10/7/18.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <cglm/vec4.h>
#include <defs.h>

#define RENDERER_CMD_BUF_SIZE       8192
#define RENDERER_VERTEX_BUF_SIZE    65536

typedef struct RenderCmd RenderCmd;
typedef struct RenderCmdBuffer RenderCmdBuffer;
typedef struct RenderCmdClear RenderCmdClear;
typedef struct RenderCmdQuads RenderCmdQuads;

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
    vec4            color;
    size_t          offset;
    size_t          count;
};

struct RenderCmdBuffer {
    u8* commands;
    u8* quadVerts;
    u8* cmdOffset;
    u8* quadVertOffset;
};

void Render_CreateCmdBuffer(RenderCmdBuffer* buf);
void Render_DestroyCmdBuffer(RenderCmdBuffer* buf);
void Render_ClearCmdBuffer(RenderCmdBuffer* buf);
void Render_PushClearCmd(RenderCmdBuffer* buf, vec4 color);
void Render_PushQuadsCmd(RenderCmdBuffer* buf, vec4 color, const float *verts, size_t count);

#endif //GAME_RENDERER_H
