//
//  Controller.h
//  Blob Game
//
//  Created by Stefan Johnson on 12/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Controller_h
#define Blob_Game_Controller_h

#include "GLSetup.h"
#include "Component.h"
#include <CommonC/Common.h>

typedef enum {
    CCControllerEventAxes,
    CCControllerEventButton
} CCControllerEvent;

typedef struct {
    double timestamp;
    _Bool down;
} CCControllerButtonState;

typedef struct {
    CCVector3D position;
} CCControllerAxesState;

typedef union {
    CCControllerAxesState axes;
    CCControllerButtonState button;
} CCControllerState;

void CCControllerSetup(void);
void CCControllerUpdateState(void);
CCControllerState CCControllerGetStateForComponent(CCComponent Component);

#endif
