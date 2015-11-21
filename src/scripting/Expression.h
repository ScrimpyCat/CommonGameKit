//
//  Expression.h
//  Blob Game
//
//  Created by Stefan Johnson on 21/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Expression_h
#define Blob_Game_Expression_h

#include <CommonC/Common.h>

typedef enum {
    CCExpressionValueTypeAtom,
    CCExpressionValueTypeInteger,
    CCExpressionValueTypeFloat,
    CCExpressionValueTypeString,
    CCExpressionValueTypeList,
    CCExpressionValueTypeExpression = CCExpressionValueTypeList
} CCExpressionValueType;

typedef struct CCExpressionValue *CCExpression;

typedef struct CCExpressionValue {
    CCExpressionValueType type;
    union {
        char *atom;
        int32_t integer;
        float real;
        char *string;
        CCOrderedCollection list;
    };
} CCExpressionValue;

CCExpression CCExpressionCreateFromSource(const char *Source);
void CCExpressionDestroy(CCExpression Expression);
void CCExpressionPrint(CCExpression Expression);

#endif
