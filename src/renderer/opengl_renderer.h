//
// Created by bison on 10/7/18.
//

#ifndef GAME_OPENGL_RENDERER_H
#define GAME_OPENGL_RENDERER_H

#include "renderer.h"

void Render_InitRendererOGL(i32 screenw, i32 screenh);
void Render_ShutdownRendererOGL();
void Render_RenderCmdBufferOGL(RenderCmdBuffer* buf);
bool Render_UploadTextureOGL(const char* filename, bool filtering, u32* tex);
void Render_DeleteTextureOGL(u32 *tex);

#endif //GAME_OPENGL_RENDERER_H
