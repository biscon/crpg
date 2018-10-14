//
// Created by bison on 10/7/18.
//

#ifndef GAME_OPENGL_RENDERER_H
#define GAME_OPENGL_RENDERER_H

#include "renderer.h"

void OGL_InitRenderer(i32 screenw, i32 screenh);
void OGL_ShutdownRenderer();
void OGL_RenderCmdBuffer(RenderCmdBuffer *buf);
bool OGL_UploadTextureGreyscale(PixelBuffer *pb, bool filtering, u32 *tex);
bool OGL_UploadTexture(PixelBuffer *pb, bool filtering, u32 *tex);
bool OGL_UploadPNGTexture(const char *filename, bool filtering, u32 *tex);
void OGL_DeleteTexture(u32 *tex);

#endif //GAME_OPENGL_RENDERER_H
