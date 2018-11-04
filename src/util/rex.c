//
// Created by bison on 11/3/18.
//

#include "rex.h"
#include <zlib.h>
#include <memory.h>
#include <SDL_log.h>

void Rex_CreateFromFile(RexImage *image, const char *filename)
{
    memset(image, 0, sizeof(RexImage));
    STORE_INIT(image->layerStore, sizeof(RexLayer));
    gzFile g = gzopen(filename, "rb");
    if(g == NULL) {
        SDL_Log("Could not create image from file %s", filename);
        return;
    }
    //Read the image attributes
    gzread(g, &image->version, sizeof(i32));
    gzread(g, &image->noLayers, sizeof(i32));
    SDL_Log("Read file format version %d, number of layers %d", image->version, image->noLayers);
    for(i32 i = 0; i < image->noLayers; ++i) {
        RexLayer layer;
        gzread(g, &layer.width, sizeof(u32));
        gzread(g, &layer.height, sizeof(u32));
        SDL_Log("Reading layer %d dims %dx%d", i, layer.width, layer.height);
        size_t layer_size = sizeof(RexTile) * layer.width * layer.height;
        layer.tiles = calloc(1, layer_size);
        gzread(g, layer.tiles, (u32) layer_size);
        STORE_PUSHBACK(image->layerStore, &layer);
    }
    gzclose(g);
}


void Rex_Destroy(RexImage *image)
{
    for(i32 i = 0; i < image->layerStore.noItems; ++i) {
        RexLayer *layer = STORE_GET_AT(image->layerStore, i);
        free(layer->tiles);
    }
    STORE_DESTROY(image->layerStore);
}
