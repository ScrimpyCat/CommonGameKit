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

#include "ExpressionEvaluator.h"
#include <inttypes.h>


typedef struct {
    char *name; //TODO: Could be put into its own hashmap
    CCExpressionEvaluator evaluator;
} CCExpressionEvaluatorInfo;

static void CCExpressionEvaluatorListElementDestructor(CCCollection Collection, CCExpressionEvaluatorInfo *Element)
{
    CC_SAFE_Free(Element->name);
}

static CCCollection EvaluatorList = NULL;
void CCExpressionEvaluatorRegister(const char *Name, CCExpressionEvaluator Evaluator)
{
    if (!EvaluatorList) EvaluatorList = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyFinding, sizeof(CCExpressionEvaluatorInfo), (CCCollectionElementDestructor)CCExpressionEvaluatorListElementDestructor);
    
    char *ComponentName = NULL;
    if (Name)
    {
        CC_SAFE_Malloc(ComponentName, sizeof(char) * (strlen(Name) + 1),
                       CC_LOG_ERROR("Failed to add new evaluator (%s) due to allocation space for name failing. Allocation size: %zu", Name, sizeof(char) * (strlen(Name) + 1));
                       return;
                       );
        
        strcpy(ComponentName, Name);
    }
    
    
    CCCollectionInsertElement(EvaluatorList, &(CCExpressionEvaluatorInfo){
        .name = ComponentName,
        .evaluator = Evaluator
    });
}

static CCComparisonResult CCExpressionEvaluatorNameComparator(const CCExpressionEvaluatorInfo *Component, const char **Name)
{
    return (Component->name) && (!strcmp(*Name, Component->name)) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

CCExpressionEvaluator CCExpressionEvaluatorForName(const char *Name)
{
    if (EvaluatorList)
    {
        CCExpressionEvaluatorInfo *Info = CCCollectionGetElement(EvaluatorList, CCCollectionFindElement(EvaluatorList, &Name, (CCComparator)CCExpressionEvaluatorNameComparator));
        
        return Info ? Info->evaluator : NULL;
    }
    
    return NULL;
}
