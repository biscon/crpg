//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include <SDL_log.h>
#include "fps_ui.h"
#include "../input/input.h"

INTERNAL InputContext inputContext;

void FPS_UI_Init()
{
    Input_CreateContext(&inputContext);
    Input_RegisterAction(&inputContext, INPUT_ACTION_UP);
    Input_RegisterAction(&inputContext, INPUT_ACTION_DOWN);

    Input_RegisterState(&inputContext, INPUT_STATE_FORWARD);
    Input_RegisterState(&inputContext, INPUT_STATE_BACK);

}

void FPS_UI_Shutdown()
{
    Input_DestroyContext(&inputContext);
}

void FPS_UI_Update() {
    InputAction action;
    if(inputContext.actions.noItems > 0) {
        Input_PollAction(&inputContext, &action);
        SDL_Log("Got action ID = %d", action.id);
    }
    if(Input_QueryState(&inputContext, INPUT_STATE_FORWARD)) {
        SDL_Log("Forwaaaaaaaaaaaaaaard");
    }
    if(Input_QueryState(&inputContext, INPUT_STATE_BACK)) {
        SDL_Log("baaaaaack");
    }
}
