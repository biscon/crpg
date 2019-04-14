//
// Created by bison on 4/9/19.
//

#ifndef GAME_UI_H
#define GAME_UI_H

#include <defs.h>
#include "game_state_ids.h"
#include "../renderer/renderer.h"

typedef struct GameState    GameState;
typedef void (*OnCreate)    ();
typedef void (*OnDestroy)   ();
typedef void (*OnStart)     ();
typedef void (*OnStop)      ();
typedef void (*OnFrame)     (RenderCmdBuffer* renderBuffer, double frameDelta);
typedef void (*OnResized)   ();
//typedef void (*SignalHandler)(int signum);


struct GameState {
    i32                     id;
    bool                    started;
    OnCreate                onCreate; // when it enters the stack
    OnDestroy               onDestroy;     // when its popped from the stack
    OnStart                 onStart;     // when its brought to the top of the stack and active
    OnStop                  onStop;     // when its no longer the top of the stack and inactive
    OnFrame                 onFrame;     // each frame while in the stack
    OnResized               onResized;     // if viewport is resized while in the stack
};

void            GameState_Init();
void            GameState_Shutdown();
void            GameState_Frame(RenderCmdBuffer *renderBuffer, double frameDelta);
void            GameState_CreateStates();
void            GameState_DestroyStates();

void            GameState_Push(GameStateId id);
void            GameState_Pop();

void            GameState_Create(GameStateId id);
void            GameState_BindOnCreate(GameStateId id, OnCreate callback);
void            GameState_BindOnDestroy(GameStateId id, OnDestroy callback);
void            GameState_BindOnStart(GameStateId id, OnStart callback);
void            GameState_BindOnStop(GameStateId id, OnStop callback);
void            GameState_BindOnFrame(GameStateId id, OnFrame callback);


#endif //GAME_UI_H
