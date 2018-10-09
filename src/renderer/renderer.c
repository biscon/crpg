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

/*
 *  Expected quad format in verts:
 *
 *  Left (v2), color (v4)
 *  Top, color
 *  Right, color
 *  Bottom, color
 *
 *  count: no of quads
 */



void Render_PushQuadsCmd(RenderCmdBuffer* buf, RenderQuad *quads, size_t count)
{
    RenderCmdQuads cmd = {.type = RCMD_QUAD};
    cmd.offset = (size_t) (buf->quadVertOffset - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * sizeof(float);
    cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
    cmd.vertexCount = count * VERTS_PER_QUAD;

    // loop through quads
    RenderQuad* q = quads;
    for(i32 i = 0; i < count; ++i) {

        float vertices[] = {
                // positions            // colors                                               // texture coords
                q->right, q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 1.0f,   // top right
                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 0.0f,   // bottom right
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 1.0f,   // top left

                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 0.0f,   // bottom right
                q->left, q->bottom,     q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 0.0f,   // bottom left
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 1.0f    // top left
        };

        memcpy(buf->quadVertOffset, &vertices, byte_size);
        buf->quadVertOffset += byte_size;
        q += sizeof(RenderQuad);
    }

    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdQuads));
    buf->cmdOffset += sizeof(RenderCmdQuads);
}



