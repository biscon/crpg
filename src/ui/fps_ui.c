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
    Input_RegisterAction(&inputContext, INPUT_ACTION_TOGGLE_FPS);
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

    while(Input_PollAction(&inputContext, &action)) {
        if(action.id == INPUT_ACTION_TOGGLE_FPS) {
            SDL_Log("Toggling FPS Display");
        }
    }

    if(Input_QueryState(&inputContext, INPUT_STATE_FORWARD)) {
        SDL_Log("Forwaaaaaaaaaaaaaaard");
    }
    if(Input_QueryState(&inputContext, INPUT_STATE_BACK)) {
        SDL_Log("baaaaaack");
    }
}
