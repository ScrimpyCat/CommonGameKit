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

#define CC_QUICK_COMPILE
#include "ExpressionEvaluator.h"
#include <inttypes.h>

CC_ARRAY_DECLARE(CCExpressionEvaluator);

static CCArray(CCExpressionEvaluator) Evaluators = NULL;
static CCDictionary(CCString, size_t) EvaluatorList = NULL;
void CCExpressionEvaluatorRegister(CCString Name, CCExpressionEvaluator Evaluator)
{
    if (!EvaluatorList)
    {
        EvaluatorList = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintSizeMedium | CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(size_t), &(CCDictionaryCallbacks){
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary,
            .keyDestructor = CCStringDestructorForDictionary
        });
        
        Evaluators = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCExpressionEvaluator), 1);
    }
    
    CCDictionarySetValue(EvaluatorList, &(CCString){ CCStringCopy(Name) }, &(size_t){ CCArrayAppendElement(Evaluators, &Evaluator) });
}

CCExpressionEvaluator CCExpressionEvaluatorForName(CCString Name)
{
    if (EvaluatorList)
    {
        size_t *Info = CCDictionaryGetValue(EvaluatorList, &Name);
        
        return Info ? *(CCExpressionEvaluator*)CCArrayGetElementAtIndex(Evaluators, *Info) : NULL;
    }
    
    return NULL;
}

CCExpressionEvaluator CCExpressionEvaluatorForIndex(size_t Index)
{
    return Evaluators ? *(CCExpressionEvaluator*)CCArrayGetElementAtIndex(Evaluators, Index) : NULL;
}

size_t CCExpressionEvaluatorIndexForName(CCString Name)
{
    if (EvaluatorList)
    {
        size_t *Info = CCDictionaryGetValue(EvaluatorList, &Name);
        
        return Info ? *Info : SIZE_MAX;
    }
    
    return SIZE_MAX;
}

CCString CCExpressionEvaluatorNameForIndex(size_t Index)
{
    //TODO: If this is a common function then do a reverse EvaluatorList (where index is key, and string is value)
    if (EvaluatorList)
    {
        CC_DICTIONARY_FOREACH_VALUE(size_t, Value, EvaluatorList)
        {
            if (Index == Value)
            {
                return *(CCString*)CCDictionaryGetKey(EvaluatorList, CCDictionaryEnumeratorGetEntry(&CC_DICTIONARY_CURRENT_VALUE_ENUMERATOR));
            }
        }
    }
    
    return 0;
}
