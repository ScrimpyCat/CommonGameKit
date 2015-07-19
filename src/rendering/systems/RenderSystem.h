//
//  RenderSystem.h
//  Blob Game
//
//  Created by Stefan Johnson on 27/06/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_RenderSystem_h
#define Blob_Game_RenderSystem_h

#include "ComponentSystem.h"

//Note: Currently just a demo system

#define CC_RENDER_SYSTEM_ID 11
#define CC_RENDER_COMPONENT_FLAG 0x1000000

void CCRenderSystemRegister(void);
void CCRenderSystemDeregister(void);

#endif
