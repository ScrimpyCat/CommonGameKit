//
//  InputSystem.h
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputSystem_h
#define Blob_Game_InputSystem_h

#include "ComponentSystem.h"

//Note: Currently just a demo system

#define CC_INPUT_SYSTEM_ID 21
#define CC_INPUT_COMPONENT_FLAG 0x2000000

void CCInputSystemRegister(void);
void CCInputSystemDeregister(void);

#endif
