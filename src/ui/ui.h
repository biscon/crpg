//
// Created by bison on 4/9/19.
//

#ifndef GAME_UI_H
#define GAME_UI_H

#include <defs.h>

typedef struct UIController UIController;

struct UIController {
    i32                     id;
    void (*onCreate)        ();
    void (*onDestroy)       ();
    void (*onLoad)          ();
    void (*onUnload)        ();
    void (*onFrame)         ();
    void (*onPaused)        ();
    void (*onResumed)       ();
    void (*onResized)       ();
};

void            UI_Init();
void            UI_Shutdown();

void            UI_PushController(i32 id);
UIController*   UI_CreateController();
void            UI_DestroyController(UIController *controller);

#endif //GAME_UI_H
