//
//  ExpressionEvaluator.c
//  Blob Game
//
//  Created by Stefan Johnson on 22/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

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
