//
// Created by bison on 4/9/19.
//

#include <list.h>
#include <store.h>
#include "game_state.h"
#include <memory.h>
#include <hashtable.h>

INTERNAL ListNode *gameStateStack;
INTERNAL hashtable_t gameStateTable;

void GameState_Init() {
    hashtable_init(&gameStateTable, sizeof(GameState), 32, NULL);
    gameStateStack = NULL;
}

void GameState_Shutdown() {
    hashtable_term(&gameStateTable);
    LIST_DESTROY(gameStateStack);
    gameStateStack = NULL;
}

void GameState_Frame(RenderCmdBuffer *renderBuffer, double frameDelta) {
    if(gameStateStack != NULL) {
        GameState* top = ((GameState*) gameStateStack->data);
        if(top->onFrame != NULL) {
            top->onFrame(renderBuffer, frameDelta);
        }
    }
}

void GameState_Create(GameStateId id) {
    GameState ctrl;
    memset(&ctrl, 0, sizeof(GameState));
    ctrl.started = false;
    hashtable_insert(&gameStateTable, (HASHTABLE_U64) id, &ctrl);
}

void GameState_BindOnFrame(GameStateId id, OnFrame callback)
{
    GameState* gs = hashtable_find(&gameStateTable, (HASHTABLE_U64) id);
    if(gs == NULL) {
        return;
    }
    gs->onFrame = callback;
}

void GameState_BindOnCreate(GameStateId id, OnCreate callback) {
    GameState* gs = hashtable_find(&gameStateTable, (HASHTABLE_U64) id);
    if(gs == NULL) {
        return;
    }
    gs->onCreate = callback;
}

void GameState_BindOnDestroy(GameStateId id, OnDestroy callback) {
    GameState* gs = hashtable_find(&gameStateTable, (HASHTABLE_U64) id);
    if(gs == NULL) {
        return;
    }
    gs->onDestroy = callback;
}

void GameState_BindOnStart(GameStateId id, OnStart callback) {
    GameState* gs = hashtable_find(&gameStateTable, (HASHTABLE_U64) id);
    if(gs == NULL) {
        return;
    }
    gs->onStart = callback;
}

void GameState_BindOnStop(GameStateId id, OnStop callback) {
    GameState* gs = hashtable_find(&gameStateTable, (HASHTABLE_U64) id);
    if(gs == NULL) {
        return;
    }
    gs->onStop = callback;
}

void GameState_Push(GameStateId id) {
    GameState* gs = hashtable_find(&gameStateTable, (HASHTABLE_U64) id);
    if(gs == NULL) {
        return;
    }

    // stop already running state
    if(gameStateStack != NULL) {
        GameState* top = ((GameState*) gameStateStack->data);
        if(top->onStop != NULL) {
            top->onStop();
            top->started = false;
        }
    }

    // push and start new state
    LIST_PUSH(gameStateStack, gs);
    if(gs->onStart != NULL) {
        gs->onStart();
        gs->started = true;
    }
}

void GameState_Pop() {
    if(gameStateStack == NULL)
        return;
    GameState *gs = LIST_POP(gameStateStack, GameState*);
    if(gs != NULL) {
        if(gs->onStop != NULL) {
            gs->onStop();
            gs->started = false;
        }
    }

    // start new top if any
    if(gameStateStack != NULL) {
        GameState* top = ((GameState*) gameStateStack->data);
        if(top != NULL && top->onStart != NULL) {
            top->onStart();
            top->started = true;
        }
    }
}

void GameState_CreateStates() {
    i32 count = hashtable_count(&gameStateTable);
    if(count < 1) {
        return;
    }
    //HASHTABLE_U64 const* keys = hashtable_keys(&gameStateTable);
    GameState* states = (GameState*) hashtable_items(&gameStateTable);
    for( int i = 0; i < count; ++i ) {
        if(states[i].onCreate != NULL) {
            states[i].onCreate();
        }
    }
}

void GameState_DestroyStates() {
    i32 count = hashtable_count(&gameStateTable);
    if(count < 1) {
        return;
    }
    //HASHTABLE_U64 const* keys = hashtable_keys(&gameStateTable);
    GameState* states = (GameState*) hashtable_items(&gameStateTable);
    for( int i = 0; i < count; ++i ) {
        // stop already started states which are in the stack
        if(states[i].started) {
            if(states[i].onStop != NULL) {
                states[i].onStop();
                states[i].started = false;
            }
        }
        if(states[i].onDestroy != NULL) {
            states[i].onDestroy();
        }
    }
}




