/*
 *  Copyright (c) 2016, Stefan Johnson
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

#define CC_QUICK_COMPILE
#include "Callbacks.h"
#include <stddef.h>
#include "ExpressionSetup.h"
#include "GFX.h"
#include "AssetManager.h"

#if CC_PLATFORM_OS_X || CC_PLATFORM_IOS
#include <CoreFoundation/CoreFoundation.h>
#endif

double (*CCTimestamp)(void) = NULL;

FSPath CCAssetPath = NULL;

static CCStringMap Map63[63] = { //ASCII set 0, [-+:&() !@<>.], [a-z], [A-G], I, [N-P], [R-T]
    0,
    '-', '+', ':', '&', '(', ')', ' ', '!', '@', '<', '>', '.',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'I', 'N', 'O', 'P', 'R', 'S', 'T'
};

static CCStringMap Map31[31] = { //ASCII set 0, -, !, @, :, ., [a-y]
    0,
    '-', '!', '@', ':', '.',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y'
};

int CCMain(CCEngineMain Main, int argc, const char *argv[])
{
    GFXShaderLibraryForName = CCAssetManagerCreateShaderLibrary;
    
    CCStringRegisterMap(CCStringEncodingASCII, Map63, CCStringMapSet63);
    CCStringRegisterMap(CCStringEncodingASCII, Map31, CCStringMapSet31);
    
    char Path[] = __FILE__;
    if ((sizeof(__FILE__) > sizeof("CommonGameKit/src/Callbacks.c")) && (!strcmp(Path + (sizeof(__FILE__) - sizeof("CommonGameKit/src/Callbacks.c")), "CommonGameKit/src/Callbacks.c")))
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"
        Path[sizeof(__FILE__) - sizeof("CommonGameKit/src/Callbacks.c")] = 0;
#pragma clang diagnostic pop
        
        CCFileFilterInputAddPath(Path);
    }
    
    CCLogAddFilter(CCLogFilterInput, CCFileFilterInput);
    CCLogAddFilter(CCLogFilterSpecifier, CCBinaryFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCArrayFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCDeletionFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCStringFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCCharFormatSpecifier);
    
    CCExpressionSetup();
    
#if CC_PLATFORM_OS_X || CC_PLATFORM_IOS
    CFBundleRef Bundle = CFBundleGetBundleWithIdentifier(CFSTR("io.scrimpycat.CommonGameKit"));
    if (Bundle)
    {
        CFURLRef ResourceURL = CFBundleCopyResourcesDirectoryURL(Bundle);
        char ResourcePath[PATH_MAX];
        
        if ((!ResourceURL) || (!CFURLGetFileSystemRepresentation(ResourceURL, TRUE, (UInt8*)ResourcePath, sizeof(ResourcePath))))
        {
            CC_LOG_ERROR("Could not find asset path");
            return EXIT_FAILURE;
        }
        
        CFRelease(ResourceURL);
        
        CCAssetPath = FSPathCreateFromSystemPath(ResourcePath);
        FSPathAppendComponent(CCAssetPath, FSPathComponentCreate(FSPathComponentTypeDirectory, "assets"));
    }
    
    else
#endif
    {
        CCOrderedCollection(FSPathComponent) Path = FSPathConvertSystemPathToComponents(argv[0], TRUE);
        CCOrderedCollectionRemoveLastElement(Path);
        FSPath ProjectFolder = FSPathCreateFromComponents(Path);
        
        CCCollection(FSPath) Paths = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall, sizeof(FSPath), FSPathDestructorForCollection);
        CCCollectionInsertElement(Paths, &(FSPath){ FSPathCreate("CommonGameKit/assets/") });
        CCOrderedCollection(FSPath) Matches = FSManagerGetContentsAtPath(ProjectFolder, Paths, FSMatchSearchRecursively);
        CCCollectionDestroy(Paths);
        FSPathDestroy(ProjectFolder);
        
        if (CCCollectionGetCount(Matches) != 1)
        {
            CCCollectionDestroy(Matches);
            CC_LOG_ERROR("Could not find asset path");
            return EXIT_FAILURE;
        }
        
        CCAssetPath = CCRetain(*(FSPath*)CCOrderedCollectionGetElementAtIndex(Matches, 0));
        CCCollectionDestroy(Matches);
    }
    
    if (!FSManagerExists(CCAssetPath))
    {
        FSPathDestroy(CCAssetPath);
        CC_LOG_ERROR("Could not find asset path");
        return EXIT_FAILURE;
    }
    
    return Main(argc, argv);
}
