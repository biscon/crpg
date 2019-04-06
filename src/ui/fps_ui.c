//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include "fps_ui.h"
#include "../input/input.h"

INTERNAL InputContext inputContext;

void FPS_UI_Init()
{
    Input_CreateContext(&inputContext);
    Input_RegisterInputAction(&inputContext, INPUT_ACTION_UP);

}

void FPS_UI_Shutdown()
{
    Input_DestroyContext(&inputContext);
}

void FPS_UI_Update() {

}
