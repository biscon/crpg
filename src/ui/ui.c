//
// Created by bison on 4/9/19.
//

#include <list.h>
#include <store.h>
#include "ui.h"
#include <memory.h>

INTERNAL ListNode *controllerStack;
INTERNAL Store controllerStore;

void UI_Init() {
    STORE_INIT(controllerStore, sizeof(UIController));
}

void UI_Shutdown() {
    LIST_DESTROY(controllerStack);
    controllerStack = NULL;
}

UIController *UI_CreateController() {
    UIController ctrl;
    memset(&ctrl, 0, sizeof(UIController));
    STORE_PUSHBACK(controllerStore, &ctrl);
    return STORE_GET_AT(controllerStore, controllerStore.noItems-1);
}
