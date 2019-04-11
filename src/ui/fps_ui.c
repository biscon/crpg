//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include <SDL_log.h>
#include "fps_ui.h"
#include "../input/input.h"

INTERNAL InputContext inputContext;
INTERNAL Font *font;
INTERNAL bool showFps = false;


void FPS_UI_Init(Font* fpsfont)
{
    font = fpsfont;
    Input_CreateContext(&inputContext);
    Input_RegisterAction(&inputContext, INPUT_ACTION_TOGGLE_FPS);
}

void FPS_UI_Shutdown()
{
    Input_DestroyContext(&inputContext);
}

void FPS_UI_Update(RenderCmdBuffer* renderBuffer, double frameDelta)
{
    InputAction action;

    while(Input_PollAction(&inputContext, &action)) {
        if(action.id == INPUT_ACTION_TOGGLE_FPS) {
            showFps = !showFps;
        }
    }

    if(showFps) {
        char buf[32];
        snprintf(buf, sizeof(buf), "FPS %.2f", 1.0 / frameDelta);
        Render_PushText(renderBuffer, font, 10, 35, COLOR_WHITE, buf);
    }
}
