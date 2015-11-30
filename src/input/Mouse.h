/*
 *  Copyright (c) 2015, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
