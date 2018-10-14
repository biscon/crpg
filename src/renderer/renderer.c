//
// Created by bison on 10/7/18.
//

#include <string.h>
#include <SDL_log.h>
#include "renderer.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define HASHTABLE_IMPLEMENTATION
#define HASHTABLE_U32 u32
#define HASHTABLE_U64 u64
#include "hashtable.h"
#include "../util/lodepng.h"
#include "opengl_renderer.h"
#include "../util/math_util.h"

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
    cmd.offset = (size_t) (buf->quadVertOffset - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * sizeof(float);
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

        memcpy(buf->quadVertOffset, &vertices, byte_size);
        buf->quadVertOffset += byte_size;
        assert(buf->quadVertOffset - buf->quadVerts > 0);
        q += sizeof(Quad);
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
    cmd.offset = (size_t) (buf->quadVertOffset - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * sizeof(float);
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

        memcpy(buf->quadVertOffset, &vertices, byte_size);
        buf->quadVertOffset += byte_size;
        assert(buf->quadVertOffset - buf->quadVerts > 0);
        q += sizeof(Quad);
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
    cmd.offset = (size_t) (buf->quadVertOffset - buf->quadVerts);
    size_t byte_size = VERTS_PER_QUAD * sizeof(float);
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

        memcpy(buf->quadVertOffset, &vertices, byte_size);
        buf->quadVertOffset += byte_size;
        assert(buf->quadVertOffset - buf->quadVerts > 0);
        q += sizeof(AtlasQuad);
    }

    memcpy(buf->cmdOffset, &cmd, sizeof(RenderCmdQuads));
    buf->cmdOffset += sizeof(RenderCmdQuads);
    assert(buf->cmdOffset - buf->commands > 0);
}


/*
 * TextureAtlas implementation (using stb_rect_pack, thanks Sean :)
 */

void TextureAtlas_Create(TextureAtlas *atlas, i32 width, i32 height)
{
    memset(atlas, 0, sizeof(TextureAtlas));
    atlas->noRects = 0;
    atlas->width = width;
    atlas->height = height;
    atlas->nextEntryId = 1;
    atlas->entryTable = calloc(1, sizeof(hashtable_t));
    hashtable_init(atlas->entryTable, sizeof(TextureAtlasEntry), 256, NULL);
    STORE_INIT(atlas->rectStore, sizeof(stbrp_rect));
}

void TextureAtlas_Destroy(TextureAtlas *atlas)
{
    if(atlas->isUploaded) {
        OGL_DeleteTexture(&atlas->textureId);
    }
    hashtable_term(atlas->entryTable);
    free(atlas->entryTable);
    STORE_DESTROY(atlas->rectStore);
}

u32 TextureAtlas_AddImage(TextureAtlas *atlas, const char *filename)
{
    TextureAtlasEntry entry;
    memset(&entry, 0, sizeof(TextureAtlasEntry));
    strncpy((void *) &(entry.filename), filename, sizeof(entry.filename));
    entry.id = atlas->nextEntryId++;
    hashtable_insert(atlas->entryTable, (HASHTABLE_U64) entry.id, &entry);
    stbrp_rect rect;
    memset(&rect, 0, sizeof(stbrp_rect));
    rect.id = entry.id;
    STORE_PUSHBACK(atlas->rectStore, &rect);
    atlas->noRects++;
    return entry.id;
}

void TextureAtlas_PackAndUpload(TextureAtlas *atlas)
{
    assert(atlas->isUploaded == false);
    stbrp_context context;
    memset(&context, 0, sizeof(stbrp_context));

    for(u32 i = 0; i < atlas->rectStore.noItems; ++i) {
        stbrp_rect* cur_rect = STORE_GET_AT(atlas->rectStore, i);
        assert(cur_rect != NULL);
        TextureAtlasEntry* entry = hashtable_find(atlas->entryTable, (u32) cur_rect->id);
        assert(entry != NULL);
        SDL_Log("Loading png %s", entry->filename);
        if(!PixelBuffer_CreateFromPNG(&entry->pixelBuffer, entry->filename)) {
            SDL_Log("Error loading png %s", entry->filename);
            continue;
        }

        cur_rect->w = (stbrp_coord) entry->pixelBuffer.width;
        cur_rect->h = (stbrp_coord) entry->pixelBuffer.height;
    }

    // TODO this might overflow the stack
    i32 nodeCount = atlas->width*2;
    struct stbrp_node nodes[nodeCount];

    stbrp_init_target(&context, atlas->width, atlas->height, nodes, nodeCount);
    stbrp_rect *rects = (stbrp_rect*) atlas->rectStore.data;
    stbrp_pack_rects(&context, rects, atlas->rectStore.noItems);
    PixelBuffer buffer;
    PixelBuffer_Create(&buffer, (u32) atlas->width, (u32) atlas->height);
    for (u32 i = 0; i < atlas->noRects; ++i)
    {
        SDL_Log("rect %i (%hu,%hu,%hu,%hu) was_packed=%i", rects[i].id, rects[i].x, rects[i].y, rects[i].w, rects[i].h, rects[i].was_packed);
        stbrp_rect* cur_rect = STORE_GET_AT(atlas->rectStore, i);
        assert(cur_rect != NULL);
        TextureAtlasEntry* entry = hashtable_find(atlas->entryTable, (u32) cur_rect->id);
        assert(entry != NULL);

        entry->uvRect.left = remapFloat(0, (float) atlas->width, 0, 1, (float) cur_rect->x);
        entry->uvRect.right = remapFloat(0, (float) atlas->width, 0, 1, (float) (cur_rect->x + cur_rect->w));
        // texture space flips y axis just to piss you off :)
        entry->uvRect.top = remapFloat(0, (float) atlas->height, 0, 1, (float) cur_rect->y);
        entry->uvRect.bottom = remapFloat(0, (float) atlas->height, 0, 1, (float) (cur_rect->y + cur_rect->h));

        PixelBuffer_SimpleBlit(&entry->pixelBuffer, (uvec4) {0, 0, entry->pixelBuffer.width,
                                                             entry->pixelBuffer.height},
                                &buffer, (uvec2) {cur_rect->x, cur_rect->y});
        PixelBuffer_Destroy(&entry->pixelBuffer);
    }

    if(!OGL_UploadTexture(&buffer, false, &atlas->textureId)) {
        SDL_Log("Texture atlas could not be uploaded");
    }
    atlas->isUploaded = true;
    PixelBuffer_Destroy(&buffer);
    STORE_DESTROY(atlas->rectStore);
}

void TextureAtlas_SetUVRect(TextureAtlas *atlas, u32 id, FloatRect *rect)
{
    assert(rect != NULL);
    TextureAtlasEntry* entry = hashtable_find(atlas->entryTable, id);
    assert(entry != NULL);
    *rect = entry->uvRect;
}



