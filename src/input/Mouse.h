//
//  Mouse.h
//  Blob Game
//
//  Created by Stefan Johnson on 10/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Mouse_h
#define Blob_Game_Mouse_h

#include "GLSetup.h"
#include "Component.h"
#include <CommonC/Common.h>

typedef enum {
    CCMouseEventMove,
    CCMouseEventButton,
    CCMouseEventScroll,
    CCMouseEventDrop
} CCMouseEvent;

typedef struct {
    double timestamp;
    _Bool down;
} CCMouseButtonState;

typedef struct {
    int32_t button;
    uint32_t flags;
    CCMouseButtonState state;
} CCMouseButtonMap;

typedef struct {
    CCVector2D delta;
} CCMouseMoveMap, CCMouseScrollMap;

typedef struct {
    CCCollection files;
} CCMouseDropItemsMap;

typedef struct {
    CCVector2D position;
    union {
        CCMouseMoveMap move;
        CCMouseButtonMap button;
        CCMouseScrollMap scroll;
        CCMouseDropItemsMap drop;
    };
} CCMouseMap;

void CCMouseDropInput(GLFWwindow *Window, int Count, const char **Files);
void CCMouseScrollInput(GLFWwindow *Window, double x, double y);
void CCMouseButtonInput(GLFWwindow *Window, int Button, int Action, int Mods);
void CCMousePositionInput(GLFWwindow *Window, double x, double y);
void CCMouseEnterInput(GLFWwindow *Window, int Entered);
CCMouseButtonState CCMouseButtonGetStateForComponent(CCComponent Component);

#endif
