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

#include "EngineSetup.h"
#include "GFXMain.h"
#include "GLGFX.h"
#include "ComponentSystem.h"
#include "EntityManager.h"
#include "ExpressionSetup.h"
#include "AssetManager.h"
#include "GUIManager.h"
#include "Configuration.h"
#include "ProjectExpressions.h"

//TODO: Probably make this file generated (could add a define so it knows what functions to call from here)
#include "RenderSystem.h"
#include "InputSystem.h"
#include "AnimationSystem.h"

#include "RenderComponent.h"
#include "InputMapKeyboardComponent.h"
#include "InputMapGroupComponent.h"
#include "InputMapMousePositionComponent.h"
#include "InputMapMouseButtonComponent.h"
#include "InputMapMouseScrollComponent.h"
#include "InputMapMouseDropComponent.h"
#include "InputMapControllerAxesComponent.h"
#include "InputMapControllerButtonComponent.h"
#include "AnimationComponent.h"
#include "AnimationKeyframeComponent.h"
#include "AnimationInterpolateComponent.h"

void AnimationInterpolator(int *Previous, int *Next, double Time, int *Result)
{
    *Result = (int)(((1.0 - Time) * (double)*Previous) + (Time * (double)*Next)); //basic lerp
}

void CCEnginePreSetup(void)
{
    char Path[] = __FILE__;
    Path[sizeof(__FILE__) - sizeof("setup/EngineSetup.c")] = 0;
    CCFileFilterInputAddPath(Path);
    CCLogAddFilter(CCLogFilterInput, CCFileFilterInput);
    CCLogAddFilter(CCLogFilterSpecifier, CCBinaryFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCArrayFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCDeletionFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCStringFormatSpecifier);
    CCLogAddFilter(CCLogFilterSpecifier, CCCharFormatSpecifier);
    
    CCExpressionSetup();
}
#include "GUIExpression.h"

static void CCEngineSetupPathElementDestructor(CCCollection Collection, FSPath *Element)
{
    FSPathDestroy(*Element);
}

void CCEngineSetup(void)
{
    GLGFXSetup();
    
    CCAssetManagerCreate();
    
    CCEntityManagerCreate();
    
    
    //Load Global Assets
    CCCollection Matches = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyEnumerating, sizeof(FSPath), (CCCollectionElementDestructor)CCEngineSetupPathElementDestructor);
    
    CCCollectionInsertElement(Matches, &(FSPath){ FSPathCreate(".gfxlib") });
    
    CC_COLLECTION_FOREACH(FSPath, Path, CCEngineConfiguration.directory.shaders)
    {
        CCOrderedCollection Paths = FSManagerGetContentsAtPath(Path, Matches, FSMatchSkipHidden | FSMatchSkipDirectory);
        
        if (Paths)
        {
            CC_COLLECTION_FOREACH(FSPath, LibPath, Paths)
            {
                CCExpression LibExpr = CCExpressionCreateFromSourceFile(LibPath);
                CCExpression Result = CCExpressionEvaluate(LibExpr);
                
                if (CCExpressionGetType(Result) == CCProjectExpressionValueTypeShaderLibrary)
                {
                    CCProjectExpressionValueShaderLibrary *Library = CCExpressionGetData(Result);
                    CCAssetManagerRegisterShaderLibrary(Library->name, Library->library);
                }
                
                CCExpressionDestroy(LibExpr);
            }
            
            CCCollectionDestroy(Paths);
        }
    }
    
    CCCollectionRemoveAllElements(Matches);
    CCCollectionInsertElement(Matches, &(FSPath){ FSPathCreate(".asset") });
    
    CCCollection GlobalAssetPaths[] = {
        CCEngineConfiguration.directory.shaders
    };
    
    for (size_t Loop = 0; Loop < sizeof(GlobalAssetPaths) / sizeof(typeof(*GlobalAssetPaths)); Loop++)
    {
        CC_COLLECTION_FOREACH(FSPath, Path, GlobalAssetPaths[Loop])
        {
            CCOrderedCollection Paths = FSManagerGetContentsAtPath(Path, Matches, FSMatchSkipHidden | FSMatchSkipDirectory);
            
            if (Paths)
            {
                CC_COLLECTION_FOREACH(FSPath, LibPath, Paths)
                {
                    CCExpression AssetExpr = CCExpressionCreateFromSourceFile(LibPath);
                    CCExpressionEvaluate(AssetExpr);
                    CCExpressionDestroy(AssetExpr);
                }
                
                CCCollectionDestroy(Paths);
            }
        }
    }
    
    
    //Register Systems
    CCRenderSystemRegister();
    CCInputSystemRegister();
    CCAnimationSystemRegister();
    
    //Register Components
    CCRenderComponentRegister();
    CCInputMapKeyboardComponentRegister();
    CCInputMapGroupComponentRegister();
    CCInputMapMousePositionComponentRegister();
    CCInputMapMouseButtonComponentRegister();
    CCInputMapMouseScrollComponentRegister();
    CCInputMapMouseDropComponentRegister();
    CCInputMapControllerAxesComponentRegister();
    CCInputMapControllerButtonComponentRegister();
    CCAnimationComponentRegister();
    CCAnimationKeyframeComponentRegister();
    CCAnimationInterpolateComponentRegister();
    
    
    //Create Managers Dependent on ECS :(
    GUIManagerCreate();
    
    
    //Load Dependent Assets
    CCCollection AssetPaths[] = {
        CCEngineConfiguration.directory.layouts
    };
    
    for (size_t Loop = 0; Loop < sizeof(AssetPaths) / sizeof(typeof(*AssetPaths)); Loop++)
    {
        CC_COLLECTION_FOREACH(FSPath, Path, AssetPaths[Loop])
        {
            CCOrderedCollection Paths = FSManagerGetContentsAtPath(Path, Matches, FSMatchSkipHidden | FSMatchSkipDirectory);
            
            if (Paths)
            {
                CC_COLLECTION_FOREACH(FSPath, LibPath, Paths)
                {
                    CCExpression AssetExpr = CCExpressionCreateFromSourceFile(LibPath);
                    CCExpressionEvaluate(AssetExpr);
                    CCExpressionDestroy(AssetExpr);
                }
                
                CCCollectionDestroy(Paths);
            }
        }
    }
    
    CCCollectionDestroy(Matches);
    
    
    //Initial Scene Setup
#warning demo
    CCEntity Player = CCEntityCreate(1, CC_STD_ALLOCATOR);
    
    CCComponent Renderer = CCComponentCreate(CC_RENDER_COMPONENT_ID);
    CCRenderComponentSetColour(Renderer, (CCColourRGB){ .r = 1.0f, .g = 0.0, .b = 0.0f });
    CCRenderComponentSetRect(Renderer, (CCRect){ .position = CCVector2DZero, .size = CCVector2DMake(1.0f, 1.0f) });
    CCEntityAttachComponent(Player, Renderer);
    CCComponentSystemAddComponent(Renderer);
    
    CCComponent Key = CCComponentCreate(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
    CCInputMapKeyboardComponentSetKeycode(Key, GLFW_KEY_SPACE);
    CCInputMapKeyboardComponentSetIsKeycode(Key, TRUE);
    CCInputMapKeyboardComponentSetIgnoreModifier(Key, TRUE);
    CCInputMapComponentSetAction(Key, "recolour");
    CCEntityAttachComponent(Player, Key);
    CCComponentSystemAddComponent(Key);
    
    CCComponent Group = CCComponentCreate(CC_INPUT_MAP_GROUP_COMPONENT_ID);
    Key = CCComponentCreate(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
    CCInputMapKeyboardComponentSetKeycode(Key, GLFW_KEY_A);
    CCInputMapKeyboardComponentSetIsKeycode(Key, TRUE);
    CCInputMapKeyboardComponentSetIgnoreModifier(Key, TRUE);
    CCInputMapKeyboardComponentSetRepeats(Key, TRUE);
    CCInputMapKeyboardComponentSetRamp(Key, 1.0);
    CCInputMapGroupComponentAddInputMap(Group, Key);
    Key = CCComponentCreate(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
    CCInputMapKeyboardComponentSetKeycode(Key, GLFW_KEY_S);
    CCInputMapKeyboardComponentSetIsKeycode(Key, TRUE);
    CCInputMapKeyboardComponentSetIgnoreModifier(Key, TRUE);
    CCInputMapKeyboardComponentSetRepeats(Key, TRUE);
    CCInputMapKeyboardComponentSetRamp(Key, 1.0);
    CCInputMapGroupComponentAddInputMap(Group, Key);
    Key = CCComponentCreate(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
    CCInputMapKeyboardComponentSetKeycode(Key, GLFW_KEY_D);
    CCInputMapKeyboardComponentSetIsKeycode(Key, TRUE);
    CCInputMapKeyboardComponentSetIgnoreModifier(Key, TRUE);
    CCInputMapKeyboardComponentSetRepeats(Key, TRUE);
    CCInputMapKeyboardComponentSetRamp(Key, 1.0);
    CCInputMapGroupComponentAddInputMap(Group, Key);
    Key = CCComponentCreate(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
    CCInputMapKeyboardComponentSetKeycode(Key, GLFW_KEY_W);
    CCInputMapKeyboardComponentSetIsKeycode(Key, TRUE);
    CCInputMapKeyboardComponentSetIgnoreModifier(Key, TRUE);
    CCInputMapKeyboardComponentSetRepeats(Key, TRUE);
    CCInputMapKeyboardComponentSetRamp(Key, 1.0);
    CCInputMapGroupComponentAddInputMap(Group, Key);
    CCInputMapComponentSetAction(Group, "move");
    CCEntityAttachComponent(Player, Group);
    CCComponentSystemAddComponent(Group);
    
    CCComponent Animation = CCComponentCreate(CC_ANIMATION_INTERPOLATE_COMPONENT_ID);
    CCAnimationComponentSetPlaying(Animation, TRUE);
    CCAnimationKeyframeComponentSetFrame(Animation, 1.0);
//    CCOrderedCollection Frames = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(double), NULL);
//    CCOrderedCollectionAppendElement(Frames, &(double){ 0.1 });
//    CCOrderedCollectionAppendElement(Frames, &(double){ 0.1 });
//    CCOrderedCollectionAppendElement(Frames, &(double){ 0.1 });
//    CCOrderedCollectionAppendElement(Frames, &(double){ 0.5 });
//    CCOrderedCollectionAppendElement(Frames, &(double){ 0.1 });
//    CCOrderedCollectionAppendElement(Frames, &(double){ 0.1 });
//    CCAnimationKeyframeComponentSetFrames(Animation, Frames);
    CCAnimationInterpolateComponentSetInterpolator(Animation, (CCAnimationInterpolator)AnimationInterpolator);
    CCOrderedCollection Data = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(int), NULL);
    CCOrderedCollectionAppendElement(Data, &(int){ 10 });
    CCOrderedCollectionAppendElement(Data, &(int){ 20 });
    CCOrderedCollectionAppendElement(Data, &(int){ 30 });
    CCOrderedCollectionAppendElement(Data, &(int){ 40 });
    CCOrderedCollectionAppendElement(Data, &(int){ 50 });
    CCOrderedCollectionAppendElement(Data, &(int){ 60 });
    CCAnimationKeyframeComponentSetData(Animation, Data);
    CCAnimationComponentSetLoop(Animation, CCAnimationLoopRepeatFlip);
    CCEntityAttachComponent(Player, Animation);
    CCComponentSystemAddComponent(Animation);
    
    CCEntityManagerAddEntity(Player);
    
    
    
    Player = CCEntityCreate(1, CC_STD_ALLOCATOR);
    Renderer = CCComponentCreate(CC_RENDER_COMPONENT_ID);
    
    CCRenderComponentSetColour(Renderer, (CCColourRGB){ .r = 0.0f, .g = 0.0, .b = 1.0f });
    CCRenderComponentSetRect(Renderer, (CCRect){ .position = CCVector2DMake(0.25f, 0.25f), .size = CCVector2DMake(0.5f, 0.5f) });
    
    CCEntityAttachComponent(Player, Renderer);
    CCEntityManagerAddEntity(Player);
    CCComponentSystemAddComponent(Renderer);
    
    
    
    
    Renderer = CCComponentCreate(CC_RENDER_COMPONENT_ID);
    
    CCRenderComponentSetColour(Renderer, (CCColourRGB){ .r = 0.0f, .g = 1.0, .b = 0.0f });
    CCRenderComponentSetRect(Renderer, (CCRect){ .position = CCVector2DMake(0.375f, 0.375f), .size = CCVector2DMake(0.25f, 0.25f) });
    
    CCEntityAttachComponent(Player, Renderer);
    CCComponentSystemAddComponent(Renderer);
    
    
    
    FSPath p = FSPathCreateFromSystemPath("~/testfoldertest/gui.scm");
    FSHandle h;
    CCExpression e = CCExpressionCreateFromSourceFile(p);

    FSPathDestroy(p);

    CCExpressionEvaluate(e);
    CCExpressionDestroy(e);


    p = FSPathCreateFromSystemPath("~/testfoldertest/usegui.scm");
    e = CCExpressionCreateFromSourceFile(p);

    FSPathDestroy(p);

    CCExpression r = CCExpressionEvaluate(e);
    if (CCExpressionGetType(r) == GUIExpressionValueTypeGUIObject)
    {
        CCExpressionChangeOwnership(r, NULL, NULL);
        GUIObject Object = CCExpressionGetData(r);
        CCExpressionDestroy(e); e = NULL;
        
        GUIManagerAddObject(Object);
        GUIObjectSetEnabled(Object, TRUE);
    }

    if (e) CCExpressionDestroy(e);

    e = NULL;
    p = NULL;
    h = NULL;
}
