//
// Created by bison on 10/7/18.
//

#ifndef GAME_OPENGL_RENDERER_H
#define GAME_OPENGL_RENDERER_H

#include "renderer.h"

void Render_InitOGLRenderer(i32 screenw, i32 screenh);
void Render_ShutdownOGLRenderer();
void Render_RenderCmdBufferOGL(RenderCmdBuffer* buf);

#endif //GAME_OPENGL_RENDERER_H
