//
// Created by bison on 10/7/18.
//

#include <string.h>
#include <SDL_log.h>
#include "renderer.h"
#include "../util/string_util.h"


void Render_CreateCmdBuffer(RenderCmdBuffer* buf)
{
    buf->commands = calloc(1, RENDERER_CMD_BUF_SIZE);
    buf->quadVerts = calloc(1, RENDERER_VERTEX_BUF_SIZE);
    buf->cmdOffset = buf->commands;
    buf->quadVertsPtr = buf->quadVerts;
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
    buf->quadVertsPtr = buf->quadVerts;
}

void Render_PushClearCmd(RenderCmdBuffer *buf, vec4 color)
{
    RenderCmdClear cmd = {.type = RCMD_CLEAR};
    memcpy(&cmd.color, color, sizeof(vec4));
    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdClear));
    buf->cmdOffset += sizeof(RenderCmdClear);
    assert(buf->cmdOffset - buf->commands > 0);
}

/*
 *  Expected quad format in verts:
 *
 *  count: no of quads
 */
void Render_PushQuadsCmd(RenderCmdBuffer* buf, Quad *quads, size_t count)
{
    RenderCmdQuads cmd = {.type = RCMD_QUAD};
    cmd.offset = (size_t) (buf->quadVertsPtr - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
    cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
    cmd.vertexCount = count * VERTS_PER_QUAD;

    // loop through quads
    Quad* q = quads;
    for(i32 i = 0; i < count; ++i) {

        float vertices[] = {
                // positions            // colors                                               // texture coords
                q->right, q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 1.0f,   // top right
                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 0.0f,   // bottom right
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 1.0f,   // top left

                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 0.0f,   // bottom right
                q->left,  q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 0.0f,   // bottom left
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 1.0f    // top left
        };

        memcpy(buf->quadVertsPtr, &vertices, byte_size);
        buf->quadVertsPtr += byte_size;
        assert(buf->quadVertsPtr - buf->quadVerts > 0);
        q += 1;
    }

    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdQuads));
    buf->cmdOffset += sizeof(RenderCmdQuads);
    assert(buf->cmdOffset - buf->commands > 0);
}

/*
 *  Expected quad format in verts:
 *
 *  count: no of quads
 */
void Render_PushTexturedQuadsCmd(RenderCmdBuffer* buf, u32 texid, Quad *quads, size_t count)
{
    RenderCmdQuads cmd = {.type = RCMD_TEX_QUAD, .texId = texid, .atlas = NULL};
    cmd.offset = (size_t) (buf->quadVertsPtr - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
    cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
    cmd.vertexCount = count * VERTS_PER_QUAD;

    // loop through quads
    Quad* q = quads;
    for(i32 i = 0; i < count; ++i) {

        float vertices[] = {
                // positions            // colors                                               // texture coords
                q->right, q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 1.0f,   // top right
                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 0.0f,   // bottom right
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 1.0f,   // top left

                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    1.0f, 0.0f,   // bottom right
                q->left,  q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 0.0f,   // bottom left
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    0.0f, 1.0f    // top left
        };

        memcpy(buf->quadVertsPtr, &vertices, byte_size);
        buf->quadVertsPtr += byte_size;
        assert(buf->quadVertsPtr - buf->quadVerts > 0);
        q += 1;
    }

    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdQuads));
    buf->cmdOffset += sizeof(RenderCmdQuads);
    assert(buf->cmdOffset - buf->commands > 0);
}



/*
 *  Expected quad format in verts:
 *
 *  count: no of quads
 */
void Render_PushAtlasQuadsCmd(RenderCmdBuffer *buf, TextureAtlas *atlas, AtlasQuad *quads,
                              size_t count)
{
    RenderCmdQuads cmd = {
        .type = RCMD_TEX_QUAD_ATLAS,
        .atlas = atlas,
        .texId = atlas->textureId
    };
    cmd.offset = (size_t) (buf->quadVertsPtr - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
    cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
    cmd.vertexCount = count * VERTS_PER_QUAD;

    // loop through quads
    AtlasQuad* q = quads;
    for(i32 i = 0; i < count; ++i) {
        TextureAtlasEntry* entry = hashtable_find(atlas->entryTable, q->atlasId);
        FloatRect* uv = &entry->uvRect;
        assert(entry != NULL);
        float vertices[] = {
                // positions            // colors                                               // texture coords
                q->right, q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    uv->right, uv->top,         // right top
                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    uv->right, uv->bottom,      // right bottom
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    uv->left, uv->top,          // left top

                q->right, q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    uv->right, uv->bottom,      // right bottom
                q->left,  q->bottom,    q->color[0], q->color[1],  q->color[2], q->color[3],    uv->left, uv->bottom,       // left bottom
                q->left,  q->top,       q->color[0], q->color[1],  q->color[2], q->color[3],    uv->left, uv->top           // left top
        };

        memcpy(buf->quadVertsPtr, &vertices, byte_size);
        buf->quadVertsPtr += byte_size;
        assert(buf->quadVertsPtr - buf->quadVerts > 0);
        q += 1;
    }

    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdQuads));
    buf->cmdOffset += sizeof(RenderCmdQuads);
    assert(buf->cmdOffset - buf->commands > 0);
}

// TODO make this more efficiently by pushing triangles directly instead of calling PushAtlasQuads (avoid a memory copy)
void Render_PushText(RenderCmdBuffer *buf, Font *font, float x, float y, vec4 color, const char *text) {
    size_t len = strlen(text);
    Store quadStore;
    STORE_INIT(quadStore, sizeof(AtlasQuad));

    for (int i=0; i < len;) {
        // TODO(Brett): if the character is null, attempt to fetch it from the font
        u32 cpLen;
        u32 cp = DecodeCodePoint(&cpLen, &text[i]);
        Glyph* c = hashtable_find(&font->glyphTable, (HASHTABLE_U64) cp);
        //assert(c != NULL);
        i += cpLen;
        if(c == NULL)
            continue;
        float xp, yp, h, w;
        xp = x + c->bearing[0];
        yp = y - c->bearing[1];
        w = c->size[0];
        h = c->size[1];
        AtlasQuad quad = {
                .color      = {color[0], color[1], color[2], color[3]},
                .atlasId    = c->atlasId,
                .left       = xp,
                .top        = yp,
                .right      = xp + w,
                .bottom     = yp + h
        };

        STORE_PUSHBACK(quadStore, &quad);
        x += c->advance;
    }

    //SDL_Log("Generated %d glyph quads", quadStore.noItems);
    Render_PushAtlasQuadsCmd(buf, &font->atlas, quadStore.data, quadStore.noItems);
    STORE_DESTROY(quadStore);
}
