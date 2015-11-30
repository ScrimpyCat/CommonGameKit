//
//  Project.h
//  Blob Game
//
//  Created by Stefan Johnson on 23/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Project_h
#define Blob_Game_Project_h

#include <CommonC/Common.h>

FSPath CCProjectCreate(const char *Directory);
void CCProjectLoad(FSPath ProjectPath);

#endif
