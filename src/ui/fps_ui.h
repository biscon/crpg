//
// Created by bison on 4/4/19.
//

#ifndef GAME_FPS_UI_H
#define GAME_FPS_UI_H

#include "../renderer/renderer.h"

void FPS_UI_Init(Font* fpsfont);
void FPS_UI_Shutdown();
void FPS_UI_Update(RenderCmdBuffer* renderBuffer, double frameDelta);

#endif //GAME_FPS_CONTROLLER_H
