//
// Created by bison on 4/3/19.
//

#include <store.h>
#include <vector.h>
#include <SDL_log.h>
#include "input.h"

INTERNAL Store mappingStore;
INTERNAL Vector inputContexts;

void Input_Init()
{
    STORE_INIT(mappingStore, sizeof(InputMapping));
    VectorAlloc(&inputContexts);

}

void Input_Shutdown()
{
    VectorFree(&inputContexts);
    STORE_DESTROY(mappingStore);
}

void Input_CreateMapping(InputMapping* mapping)
{
    STORE_PUSHBACK(mappingStore, mapping);
}

void Input_CreateContext(InputContext *context)
{
    STORE_INIT(context->actionIds, sizeof(MappedInputId));
    STORE_INIT(context->stateIds, sizeof(MappedInputId));
    STORE_INIT(context->actions, sizeof(InputAction));
    STORE_INIT(context->states, sizeof(InputState));
    VECTOR_ADD(inputContexts, context);
}

void Input_DestroyContext(InputContext *context)
{
    STORE_DESTROY(context->actionIds);
    STORE_DESTROY(context->stateIds);
    STORE_DESTROY(context->actions);
    STORE_DESTROY((context->states));
    VECTOR_REMOVE_ITEM(inputContexts, context);
}

void Input_RegisterAction(InputContext *context, MappedInputId id)
{
    STORE_PUSHBACK(context->actionIds, &id);
}

void Input_RegisterState(InputContext *context, MappedInputId id)
{
    STORE_PUSHBACK(context->stateIds, &id);
}


INTERNAL InputState* findOrCreateState(InputContext* context, InputMapping* mapping)
{
    for(i32 i = 0; i < context->states.noItems; i++) {
        InputState *curstate = STORE_GET_AT(context->states, i);
        if(curstate->id == mapping->mappedId) {
            return curstate;
        }
    }
    InputState state;
    state.id = mapping->mappedId;
    STORE_PUSHBACK(context->states, &state);
    return STORE_GET_AT(context->states, context->states.noItems-1);
}

/*
 * Finds contexts that have registered for this particular input mapping
 */
INTERNAL void dispatchActionMapping(InputMapping* mapping)
{
    for(i32 i = 0; i < inputContexts.total; i++) {
        InputContext *context = VECTOR_GET(inputContexts, InputContext*, i);
        for(i32 j = 0; j < context->actionIds.noItems; j++) {
            MappedInputId* curid = STORE_GET_AT(context->actionIds, j);
            if(*curid == mapping->mappedId) {
                InputAction action;
                action.id = mapping->mappedId;
                STORE_PUSHBACK(context->actions, &action);
            }
        }
    }
}

/*
 * Finds contexts that have registered for this particular input mapping
 */
INTERNAL void dispatchStateMapping(InputMapping* mapping, bool active)
{
    for(i32 i = 0; i < inputContexts.total; i++) {
        InputContext *context = VECTOR_GET(inputContexts, InputContext*, i);
        for(i32 j = 0; j < context->stateIds.noItems; j++) {
            MappedInputId* curid = STORE_GET_AT(context->stateIds, j);
            if(*curid == mapping->mappedId) {
                InputState* state = findOrCreateState(context, mapping);
                state->active = active;
            }
        }
    }
}

void Input_PushKeyUp(SDL_Keycode keycode)
{
    InputMapping* mapping;
    for(i32 i = 0; i < mappingStore.noItems; i++) {
        mapping = STORE_GET_AT(mappingStore, i);
        if(mapping->event.type == RIET_KEYBOARD &&
           mapping->event.keycode == keycode &&
           mapping->type == IMT_STATE) {
            //SDL_Log("Dispatching state mapping false");
            dispatchStateMapping(mapping, false);
        }
    }
}

void Input_PushKeyDown(SDL_Keycode keycode)
{
    InputMapping* mapping;
    for(i32 i = 0; i < mappingStore.noItems; i++) {
        mapping = STORE_GET_AT(mappingStore, i);
        if(mapping->event.type == RIET_KEYBOARD &&
           mapping->event.keycode == keycode &&
           mapping->type == IMT_ACTION) {
            //SDL_Log("Dispatching action mapping");
            dispatchActionMapping(mapping);
        }
        if(mapping->event.type == RIET_KEYBOARD &&
           mapping->event.keycode == keycode &&
           mapping->type == IMT_STATE) {
            //SDL_Log("Dispatching state mapping true");
            dispatchStateMapping(mapping, true);
        }
    }
}

void Input_PollAction(InputContext *context, InputAction *action)
{
    InputAction* src = STORE_GET_AT(context->actions, context->actions.noItems-1);
    memcpy(action, src, sizeof(InputAction));
    STORE_REMOVE_AT(context->actions, context->actions.noItems-1);
}

bool Input_QueryState(InputContext *context, u32 id) {
    for(i32 i = 0; i < context->states.noItems; ++i) {
        InputState *state = STORE_GET_AT(context->states, i);
        if(state->id == id) {
            return state->active;
        }
    }
    return false;
}
