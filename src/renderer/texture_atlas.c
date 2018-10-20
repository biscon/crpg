//
// Created by bison on 10/16/18.
//

#include <assert.h>
#include <defs.h>
#include <hashtable.h>
#include <SDL_log.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#include "texture_atlas.h"
#include "opengl_renderer.h"
#include "../util/math_util.h"

/*
 * TextureAtlas implementation (using stb_rect_pack, thanks Sean :)
 */

void TextureAtlas_Create(TextureAtlas *atlas, i32 width, i32 height, PixelBufferFormat format)
{
    memset(atlas, 0, sizeof(TextureAtlas));
    atlas->noRects = 0;
    atlas->width = width;
    atlas->height = height;
    atlas->nextEntryId = 1;
    atlas->format = format;
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

u32 TextureAtlas_AddImageFromPNG(TextureAtlas *atlas, const char *filename)
{
    TextureAtlasEntry entry;
    memset(&entry, 0, sizeof(TextureAtlasEntry));
    SDL_Log("Loading png %s", filename);
    PixelBuffer_CreateFromPNG(&entry.pixelBuffer, filename);
    entry.id = atlas->nextEntryId++;
    hashtable_insert(atlas->entryTable, (HASHTABLE_U64) entry.id, &entry);
    stbrp_rect rect;
    memset(&rect, 0, sizeof(stbrp_rect));
    rect.id = entry.id;
    rect.w = (stbrp_coord) entry.pixelBuffer.width;
    rect.h = (stbrp_coord) entry.pixelBuffer.height;
    STORE_PUSHBACK(atlas->rectStore, &rect);
    atlas->noRects++;
    return entry.id;
}

u32 TextureAtlas_AddImage(TextureAtlas *atlas, PixelBuffer* src_buf)
{
    TextureAtlasEntry entry;
    memset(&entry, 0, sizeof(TextureAtlasEntry));
    PixelBuffer_Create(&entry.pixelBuffer, src_buf->width, src_buf->height, src_buf->format);
    PixelBuffer_SimpleBlit(src_buf, (uvec4) {0,0,src_buf->width,src_buf->height},
                           &entry.pixelBuffer, (uvec4) {0,0,src_buf->width,src_buf->height});
    entry.id = atlas->nextEntryId++;
    hashtable_insert(atlas->entryTable, (HASHTABLE_U64) entry.id, &entry);
    stbrp_rect rect;
    memset(&rect, 0, sizeof(stbrp_rect));
    rect.id = entry.id;
    rect.w = (stbrp_coord) entry.pixelBuffer.width;
    rect.h = (stbrp_coord) entry.pixelBuffer.height;
    STORE_PUSHBACK(atlas->rectStore, &rect);
    atlas->noRects++;
    return entry.id;
}

void TextureAtlas_PackAndUpload(TextureAtlas *atlas) {
    assert(atlas->isUploaded == false);
    stbrp_context context;
    memset(&context, 0, sizeof(stbrp_context));

    // TODO this might overflow the stack
    i32 nodeCount = atlas->width * 2;
    struct stbrp_node nodes[nodeCount];

    stbrp_init_target(&context, atlas->width, atlas->height, nodes, nodeCount);
    stbrp_rect *rects = (stbrp_rect *) atlas->rectStore.data;
    stbrp_pack_rects(&context, rects, atlas->rectStore.noItems);
    PixelBuffer buffer;
    PixelBuffer_Create(&buffer, (u32) atlas->width, (u32) atlas->height, atlas->format);
    for (u32 i = 0; i < atlas->noRects; ++i) {
        /*
        SDL_Log("rect %i (%hu,%hu,%hu,%hu) was_packed=%i", rects[i].id, rects[i].x, rects[i].y,
                rects[i].w, rects[i].h, rects[i].was_packed);
        */
        stbrp_rect *cur_rect = STORE_GET_AT(atlas->rectStore, i);
        assert(cur_rect != NULL);
        TextureAtlasEntry *entry = hashtable_find(atlas->entryTable, (u32) cur_rect->id);
        assert(entry != NULL);

        entry->uvRect.left = remapFloat(0, (float) atlas->width, 0, 1, (float) cur_rect->x);
        entry->uvRect.right = remapFloat(0, (float) atlas->width, 0, 1,
                                         (float) (cur_rect->x + cur_rect->w));
        // texture space flips y axis just to piss you off :)
        entry->uvRect.top = remapFloat(0, (float) atlas->height, 0, 1, (float) cur_rect->y);
        entry->uvRect.bottom = remapFloat(0, (float) atlas->height, 0, 1,
                                          (float) (cur_rect->y + cur_rect->h));

        PixelBuffer_SimpleBlit(&entry->pixelBuffer, (uvec4) {0, 0, entry->pixelBuffer.width,
                                                             entry->pixelBuffer.height},
                               &buffer, (uvec2) {cur_rect->x, cur_rect->y});
        PixelBuffer_Destroy(&entry->pixelBuffer);
    }

    if (atlas->format == PBF_RGBA) {
        if (!OGL_UploadTexture(&buffer, false, &atlas->textureId)) {
            SDL_Log("Texture atlas could not be uploaded");
        }
    } else {
        if (!OGL_UploadTextureGreyscale(&buffer, true, &atlas->textureId)) {
            SDL_Log("Greyscale texture atlas could not be uploaded");
        }
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
