//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include <SDL_log.h>
#include "game.h"
#include "../input/input.h"
#include "game_state.h"
#include "intro_state.h"
#include "combat_state.h"

INTERNAL InputContext inputContext;
INTERNAL Font font;
INTERNAL bool showFps = false;

INTERNAL void SetupInput() {
    // Actions -------------------------------------------------------------------------------------
    InputMapping mapping;
    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_SELECT;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_RETURN;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_ESCAPE;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_ESCAPE;
    Input_CreateMapping(&mapping);



    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_TOGGLE_FPS;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_F1;
    Input_CreateMapping(&mapping);

    // Arrow keys
    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_UP;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_UP;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_DOWN;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_DOWN;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_LEFT;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_LEFT;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_ACTION;
    mapping.mappedId = INPUT_ACTION_RIGHT;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_RIGHT;
    Input_CreateMapping(&mapping);

    // States --------------------------------------------------------------------------------------
    mapping.type = IMT_STATE;
    mapping.mappedId = INPUT_STATE_FORWARD;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_w;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_STATE;
    mapping.mappedId = INPUT_STATE_BACK;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_s;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_STATE;
    mapping.mappedId = INPUT_STATE_LEFT;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_a;
    Input_CreateMapping(&mapping);

    mapping.type = IMT_STATE;
    mapping.mappedId = INPUT_STATE_RIGHT;
    mapping.event.type = RIET_KEYBOARD;
    mapping.event.keycode = SDLK_d;
    Input_CreateMapping(&mapping);
}

void Game_Init()
{
    SetupInput();

    Font_Create(&font, "assets/PressStart2P.ttf", 24);
    //Font_Create(&font, "assets/Inconsolata.otf", 24);
    //Font_Create(&font, "assets/bigblue437.ttf", 18);
    //Font_Create(&font, "assets/OpenSans-Regular.ttf", 24);
    Input_CreateContext(&inputContext);
    Input_RegisterAction(&inputContext, INPUT_ACTION_TOGGLE_FPS);
    Input_RegisterAction(&inputContext, INPUT_ACTION_ESCAPE);
    Input_RegisterAction(&inputContext, INPUT_ACTION_UP);

    GameState_Init();
    IntroState_Register();
    CombatState_Register();
    GameState_CreateStates();
    // Push Initial Game State
    GameState_Push(GAME_STATE_INTRO);
    //GameState_Push(GAME_STATE_COMBAT);
}

void Game_Shutdown()
{
    GameState_DestroyStates();
    GameState_Shutdown();

    Input_DestroyContext(&inputContext);
    Font_Destroy(&font);
}

bool Game_Update(RenderCmdBuffer *renderBuffer, double frameDelta)
{
    InputAction action;

    while(Input_PollAction(&inputContext, &action)) {
        if(action.id == INPUT_ACTION_TOGGLE_FPS) {
            showFps = !showFps;
        }
        if(action.id == INPUT_ACTION_ESCAPE) {
            GameState_Pop();
            if(GameState_IsStackEmpty()) {
                SDL_Log("QUIT!!!! FOR HELVEDE!!!");
                return true;
            }
        }
        if(action.id == INPUT_ACTION_UP) {
            GameState_Push(GAME_STATE_COMBAT);
        }
    }

    GameState_Frame(renderBuffer, frameDelta);

    if(showFps) {
        char buf[32];
        snprintf(buf, sizeof(buf), "FPS %.2f", 1.0 / frameDelta);
        Render_PushText(renderBuffer, &font, 10, 35, COLOR_WHITE, buf);
    }
    return false;
}
