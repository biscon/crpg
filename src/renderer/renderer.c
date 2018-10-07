//
// Created by bison on 10/7/18.
//

#include <string.h>
#include <SDL_log.h>
#include "renderer.h"

void Render_CreateCmdBuffer(RenderCmdBuffer* buf)
{
    buf->commands = calloc(1, RENDERER_CMD_BUF_SIZE);
    buf->quadVerts = calloc(1, RENDERER_VERTEX_BUF_SIZE);
    buf->cmdOffset = buf->commands;
    buf->quadVertOffset = buf->quadVerts;
}

void Render_DestroyCmdBuffer(RenderCmdBuffer* buf)
{
    if(buf->commands)
        free(buf->commands);
    if(buf->quadVerts)
        free(buf->quadVerts);
}

void Render_ClearCmdBuffer(RenderCmdBuffer *buf)
{
    memset(buf->commands, 0, RENDERER_CMD_BUF_SIZE);
    memset(buf->quadVerts, 0, RENDERER_VERTEX_BUF_SIZE);
    buf->cmdOffset = buf->commands;
    buf->quadVertOffset = buf->quadVerts;
}

void Render_PushClearCmd(RenderCmdBuffer *buf, vec4 color)
{
    RenderCmdClear cmd = {.type = RCMD_CLEAR};
    memcpy(&cmd.color, color, sizeof(vec4));
    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdClear));
    buf->cmdOffset += sizeof(RenderCmdClear);
}

void Render_PushQuadsCmd(RenderCmdBuffer* buf, vec4 color, const float *verts, size_t count)
{
    RenderCmdQuads cmd = {.type = RCMD_QUAD};
    memcpy(&cmd.color, color, sizeof(vec4));
    cmd.offset = (size_t) (buf->quadVertOffset - buf->quadVerts);
    cmd.count = count;

    size_t byte_size = count * (6* sizeof(float));
    memcpy(buf->quadVertOffset, verts, byte_size);
    buf->quadVertOffset += byte_size;

    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdQuads));
    buf->cmdOffset += sizeof(RenderCmdQuads);
}



