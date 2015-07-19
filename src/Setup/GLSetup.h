//
//  GLSetup.h
//  Blob Game
//
//  Created by Stefan Johnson on 5/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_GLSetup_h
#define Blob_Game_GLSetup_h

#include <CommonC/Common.h>

#if CC_PLATFORM_OS_X
#define GLFW_INCLUDE_GLCOREARB
#else
#include <gl_core.h>
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

#endif
