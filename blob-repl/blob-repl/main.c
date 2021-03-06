/*
 *  Copyright (c) 2017, Stefan Johnson
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

#include <CommonGameKit/CommonGameKit.h>

#if CC_PLATFORM_APPLE || CC_PLATFORM_UNIX
#define USE_READLINE 1
#endif

#if USE_READLINE
#include <readline/readline.h>
#else
static char Buffer[16384];
#endif

#if CC_PLATFORM_APPLE
#include <mach/mach_time.h>

double TimebaseSeconds;
static double CurrentAbsoluteTime(void)
{
    return (double)mach_absolute_time() * TimebaseSeconds;
}
#endif

static double AssertTime(void)
{
    CCAssertLog(0, "Implementation missing for CCTimestamp");
    return 0.0;
}

static void Setup(void)
{
#if CC_PLATFORM_APPLE
    static mach_timebase_info_data_t Timebase;
    if (!mach_timebase_info(&Timebase)) {
        TimebaseSeconds = 1e-9 * (double)Timebase.numer / (double)Timebase.denom;
        CCTimestamp = CurrentAbsoluteTime;
    }
#endif
    
    if (!CCTimestamp) CCTimestamp = AssertTime;
    
    CCEntityManagerCreate();
    
//    //Register Systems
//    CCRenderSystemRegister();
//    CCInputSystemRegister();
//    CCAnimationSystemRegister();
//
//    //Register Components
//    CCRenderComponentRegister();
//    CCInputMapKeyboardComponentRegister();
//    CCInputMapGroupComponentRegister();
//    CCInputMapMousePositionComponentRegister();
//    CCInputMapMouseButtonComponentRegister();
//    CCInputMapMouseScrollComponentRegister();
//    CCInputMapMouseDropComponentRegister();
//    CCInputMapControllerAxesComponentRegister();
//    CCInputMapControllerButtonComponentRegister();
//    CCAnimationComponentRegister();
//    CCAnimationKeyframeComponentRegister();
//    CCAnimationInterpolateComponentRegister();
//
//    GUIManagerCreate();
}

int Repl(int argc, const char *argv[])
{
    Setup();
    
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCString), CCStringDestructorForCollection);
    CCOrderedCollectionAppendElement(Strings, &(CCString){ CCStringCopy(CC_STRING("(begin")) });
        
    size_t Line = 1, ExpressionCount = 0;
    _Bool IsString = FALSE, BackSlash = FALSE;
    while (TRUE)
    {
        char Prompt[23];
        sprintf(Prompt, "%zu> ", Line++);
        
#if USE_READLINE
        char *Buffer = readline(Prompt);
        add_history(Buffer);
#else
        printf(Prompt);
#endif
        
#if !USE_READLINE
        if (scanf("%16383[^\n]%*c", Buffer) == 1)
#endif
        {
            size_t Length = strlen(Buffer);
            
            for (size_t Loop = 0; Loop < Length; Loop++)
            {
                const char c = Buffer[Loop];
                if (IsString)
                {
                    if (BackSlash) BackSlash = FALSE;
                    else if (c == '"') IsString = FALSE;
                    else if (c == '\\') BackSlash = TRUE;
                }
                
                else if (c == '(') ExpressionCount++;
                else if (c == ')') ExpressionCount--;
                else if (c == '"') IsString = TRUE;
            }
            
            CCOrderedCollectionAppendElement(Strings, &(CCString){ CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingASCII | CCStringHintCopy, Buffer, Length) });
            
            if (!ExpressionCount)
            {
                CCOrderedCollectionAppendElement(Strings, &(CCString){ CCStringCopy(CC_STRING(")")) });
                CCString Input = CCStringCreateByJoiningEntries(Strings, CC_STRING("\n"));
                
                CC_STRING_TEMP_BUFFER(Buf, Input)
                {
                    CCExpression Expression = CCExpressionCreateFromSource(Buf);
                    if (Expression)
                    {
                        CCExpression Result = CCExpressionEvaluate(Expression);
                        CCExpressionPrint(Result);
                        CCExpressionDestroy(Expression);
                    }
                    
                    else puts("Invalid expression");
                }
                
                CCStringDestroy(Input);
                CCCollectionRemoveAllElements(Strings);
                
                CCOrderedCollectionAppendElement(Strings, &(CCString){ CCStringCopy(CC_STRING("(begin")) });
                
                IsString = FALSE;
                BackSlash = FALSE;
            }
            
#if USE_READLINE
            free(Buffer);
#endif
        }

#if !USE_READLINE
        else
        {
            scanf("%*[^\n]");
            scanf("%*c");
        }
#endif
    }
    
    return 0;
}

int main(int argc, const char *argv[])
{
    CCMain(Repl, argc, argv);
    
    return 0;
}
