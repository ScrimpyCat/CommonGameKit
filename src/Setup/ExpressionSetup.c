//
//  ExpressionSetup.c
//  Blob Game
//
//  Created by Stefan Johnson on 22/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#include "ExpressionSetup.h"
#include "ExpressionEvaluator.h"

#include "MathExpressions.h"

void CCExpressionSetup(void)
{
    CCExpressionEvaluatorRegister("+", CCMathExpressionAddition);
    CCExpressionEvaluatorRegister("-", CCMathExpressionSubtract);
    CCExpressionEvaluatorRegister("*", CCMathExpressionMultiply);
    CCExpressionEvaluatorRegister("/", CCMathExpressionDivide);
}
