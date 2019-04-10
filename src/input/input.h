//
// Created by bison on 4/3/19.
//

#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <defs.h>
#include <SDL_keycode.h>
#include "mapped_input.h"
#include <store.h>

typedef struct InputContext         InputContext;
typedef struct InputAction          InputAction;
typedef struct InputMapping         InputMapping;
typedef struct InputState           InputState;
typedef struct RawInputEvent        RawInputEvent;

typedef enum {
    RIET_KEYBOARD,
    RIET_MOUSE,
    RIET_CONTROLLER
} RawInputEventType;

struct RawInputEvent {
    RawInputEventType               type;
    SDL_Keycode                     keycode;
};


typedef enum {
                                    IMT_ACTION,
                                    IMT_STATE
} InputMappingType;

struct InputContext {
    Store                           actionIds;
    Store                           stateIds;
    Store                           actions;
    Store                           states;
};

struct InputAction {
    MappedInputId                   id;
};

struct InputState {
    MappedInputId                   id;
    bool                            active;
};

struct InputMapping {
    InputMappingType                type;
    MappedInputId                   mappedId;
    RawInputEvent                   event;
};


void Input_Init();
void Input_Shutdown();
// TODO make config module call this (load bindings from a file)
void Input_CreateMapping(InputMapping *mapping);


// used by the application layer to poll for mapped input associated with a context
void Input_CreateContext(InputContext* context);
void Input_DestroyContext(InputContext* context);
void Input_RegisterAction(InputContext *context, MappedInputId id);
void Input_RegisterState(InputContext *context, MappedInputId id);

bool Input_PollAction(InputContext* context, InputAction* action);
bool Input_QueryState(InputContext* context, u32 id);

// Call these functions to feed the input layer with raw input
void Input_PushKeyUp(SDL_Keycode keycode);
void Input_PushKeyDown(SDL_Keycode keycode);

#endif //GAME_INPUT_H
