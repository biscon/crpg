//
// Created by bison on 4/4/19.
//

#ifndef GAME_H
#define GAME_H

#include "../renderer/renderer.h"

void Game_Init();
void Game_Shutdown();
bool Game_Update(RenderCmdBuffer *renderBuffer, double frameDelta);

#endif
