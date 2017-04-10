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

static char Buffer[16384];
int Repl(int argc, const char *argv[])
{
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCString), CCStringDestructorForCollection);
    CCOrderedCollectionAppendElement(Strings, &(CCString){ CCStringCopy(CC_STRING("(begin")) });
        
    size_t Line = 1, ExpressionCount = 0;
    _Bool IsString = FALSE, BackSlash = FALSE;
    while (TRUE)
    {
        printf("%zu> ", Line++);
        
        if (scanf("%16383[^\n]%*c", Buffer) == 1)
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
        }
        
        else
        {
            scanf("%*[^\n]");
            scanf("%*c");
        }
    }
    
    return 0;
}

int main(int argc, const char *argv[])
{
    CCMain(Repl, argc, argv);
    
    return 0;
}
