/*
 *  Copyright (c) 2024, Stefan Johnson
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

#import "CCTestCase.h"
#import "ExpressionMap.h"

@interface ExpressionMapTests : CCTestCase

@end

static _Bool CustomMap(CCExpression ArgExpr, CCEnumerator *Enumerator, CCOrderedCollection Results)
{
    if ((CCExpressionGetType(ArgExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetInteger(ArgExpr) == 123))
    {
        CCOrderedCollectionAppendElement(Results, &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":mapped-custom"), TRUE) });
        
        return TRUE;
    }
    
    return FALSE;
}

@implementation ExpressionMapTests

-(void) testMap
{
    CCExpression Value = CCExpressionCreateFromSource(
        "(foo"
        "    (some-value: 3)"
        "    (some-value: :test)"
        "    (some-value: :one-hundred)"
        "    (some-value: 5)"
        "    (multi-value: 0)"
        "    (multi-value: 1 2)"
        "    (multi-value: 10 20 30)"
        "    (some-value: 123)"
        ") "
    );
    
    CCDictionary Map = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, sizeof(CCString), sizeof(CCExpressionMapArg**), &(CCDictionaryCallbacks){
        .keyDestructor = CCStringDestructorForDictionary,
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary
    });
    
    CCDictionary Enums = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, sizeof(CCString), sizeof(uintmax_t), &(CCDictionaryCallbacks){
        .keyDestructor = CCStringDestructorForDictionary,
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary
    });
    
    CCDictionarySetValue(Enums, &(CCString){ CC_STRING(":one-hundred") }, &(uintmax_t){ 100 });
    CCDictionarySetValue(Enums, &(CCString){ CC_STRING(":two-hundred") }, &(uintmax_t){ 200 });
    
    CCDictionarySetValue(Map, &(CCString){ CC_STRING("some-value:") }, &(CCExpressionMapArg**){ (CCExpressionMapArg*[]){
        &(CCExpressionMapArg){
            .name = CC_STRING("some-value:"),
            .match = NULL,
            .map = CustomMap
        },
        &(CCExpressionMapArg){
            .name = CC_STRING("some-value:"),
            .match = &(CCExpressionMapMatch){ .type = CCExpressionMapMatchTypeInteger, .integer = 3 },
            .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("some-value-match:"), .value = NULL }, &(CCExpressionMapTo){ .name = CC_STRING("some-value-match-to-different:"), .value = CCExpressionCreateInteger(CC_STD_ALLOCATOR, 1) }, NULL }
        },
        &(CCExpressionMapArg){
            .name = CC_STRING("some-value:"),
            .match = &(CCExpressionMapMatch){ .type = CCExpressionMapMatchTypeAtom, .atom = CC_STRING(":test") },
            .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("some-value-match-atom:"), .value = NULL }, NULL }
        },
        &(CCExpressionMapArg){
            .name = CC_STRING("some-value:"),
            .match = &(CCExpressionMapMatch){ .type = CCExpressionMapMatchTypeEnums, .enums = Enums },
            .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("some-value-match-enum:"), .value = NULL }, NULL }
        },
        &(CCExpressionMapArg){
            .name = CC_STRING("some-value:"),
            .match = &(CCExpressionMapMatch){ .type = CCExpressionMapMatchTypeAlways },
            .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("some-value-catch-all:"), .value = NULL }, NULL }
        },
        NULL
    } });
    
    CCDictionarySetValue(Map, &(CCString){ CC_STRING("multi-value:") }, &(CCExpressionMapArg**){ (CCExpressionMapArg*[]){
        &(CCExpressionMapArg){
            .name = CC_STRING("multi-value:"),
            .match = &(CCExpressionMapMatch){
                .type = CCExpressionMapMatchTypeArgs,
                .args = (const CCExpressionMapArg*[]){
                    &(CCExpressionMapArg){
                        .name = CC_STRING("x"),
                        .match = &(CCExpressionMapMatch){ .type = CCExpressionMapMatchTypeAlways },
                        .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("x:"), .value = NULL }, NULL }
                    },
                    &(CCExpressionMapArg){
                        .name = CC_STRING("y"),
                        .match = &(CCExpressionMapMatch){ .type = CCExpressionMapMatchTypeAlways },
                        .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("y:"), .value = NULL }, NULL }
                    },
                    NULL
                }
            },
            .to = (const CCExpressionMapTo*[]){ &(CCExpressionMapTo){ .name = CC_STRING("point:"), .value = NULL }, NULL }
        },
        NULL
    } });
    
    CCExpression Result = CCExpressionMap(Value, Map);
    CCExpressionPrint(Result);
    CCExpression Check = CCExpressionCreateFromSource(
        "(= 1"
        "   ((some-value-match: 3) (some-value-match-to-different: 1) (some-value-match-atom: :test) (some-value-match-enum: 100) (some-value-catch-all: 5) (y: 2) (x: 1) (point: 1) (y: 20) (x: 10) (point: 10) :mapped-custom)"
        ") "
    );
    
    CCOrderedCollectionReplaceElementAtIndex(CCExpressionGetList(Check), &Result, 1);
    Result = CCExpressionEvaluate(Check);
    
    XCTAssertTrue(CCExpressionGetInteger(Result), @"Should map the values correctly");
    
    CCExpressionDestroy(Check);
    CCExpressionDestroy(Value);
    
    CCDictionaryDestroy(Map);
    CCDictionaryDestroy(Enums);
}

@end
