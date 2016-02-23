//This file is automatically generated, modifications will be lost!
#include "ExpressionSetup.h"
#include "ExpressionEvaluator.h"

#include "GUIExpression.h"
#include "ControlFlowExpressions.h"
#include "EqualityExpressions.h"
#include "IOExpressions.h"
#include "ListExpressions.h"
#include "MacroExpressions.h"
#include "MathExpressions.h"
#include "ProjectExpressions.h"
#include "StateExpressions.h"
#include "StringExpressions.h"
#include "WindowExpressions.h"

void CCExpressionSetup(void)
{
    CCExpressionEvaluatorRegister(CC_STRING("gui"), GUIExpressionRegisterObject);
    CCExpressionEvaluatorRegister(CC_STRING("percent-width"), GUIExpressionPercentWidth);
    CCExpressionEvaluatorRegister(CC_STRING("percent-height"), GUIExpressionPercentHeight);
    CCExpressionEvaluatorRegister(CC_STRING("begin"), CCControlFlowExpressionBegin);
    CCExpressionEvaluatorRegister(CC_STRING("if"), CCControlFlowExpressionBranch);
    CCExpressionEvaluatorRegister(CC_STRING("loop"), CCControlFlowExpressionLoop);
    CCExpressionEvaluatorRegister(CC_STRING("="), CCEqualityExpressionEqual);
    CCExpressionEvaluatorRegister(CC_STRING("!="), CCEqualityExpressionNotEqual);
    CCExpressionEvaluatorRegister(CC_STRING("<="), CCEqualityExpressionLessThanEqual);
    CCExpressionEvaluatorRegister(CC_STRING(">="), CCEqualityExpressionGreaterThanEqual);
    CCExpressionEvaluatorRegister(CC_STRING("<"), CCEqualityExpressionLessThan);
    CCExpressionEvaluatorRegister(CC_STRING(">"), CCEqualityExpressionGreaterThan);
    CCExpressionEvaluatorRegister(CC_STRING("print"), CCIOExpressionPrint);
    CCExpressionEvaluatorRegister(CC_STRING("search"), CCIOExpressionSearch);
    CCExpressionEvaluatorRegister(CC_STRING("get"), CCListExpressionGetter);
    CCExpressionEvaluatorRegister(CC_STRING("quote"), CCMacroExpressionQuote);
    CCExpressionEvaluatorRegister(CC_STRING("unquote"), CCMacroExpressionUnquote);
    CCExpressionEvaluatorRegister(CC_STRING("+"), CCMathExpressionAddition);
    CCExpressionEvaluatorRegister(CC_STRING("-"), CCMathExpressionSubtract);
    CCExpressionEvaluatorRegister(CC_STRING("*"), CCMathExpressionMultiply);
    CCExpressionEvaluatorRegister(CC_STRING("/"), CCMathExpressionDivide);
    CCExpressionEvaluatorRegister(CC_STRING("min"), CCMathExpressionMinimum);
    CCExpressionEvaluatorRegister(CC_STRING("max"), CCMathExpressionMaximum);
    CCExpressionEvaluatorRegister(CC_STRING("random"), CCMathExpressionRandom);
    CCExpressionEvaluatorRegister(CC_STRING("game"), CCProjectExpressionGame);
    CCExpressionEvaluatorRegister(CC_STRING("library"), CCProjectExpressionLibrary);
    CCExpressionEvaluatorRegister(CC_STRING("source"), CCProjectExpressionLibrarySource);
    CCExpressionEvaluatorRegister(CC_STRING("state!"), CCStateExpressionCreateState);
    CCExpressionEvaluatorRegister(CC_STRING("enum!"), CCStateExpressionCreateEnum);
    CCExpressionEvaluatorRegister(CC_STRING("super"), CCStateExpressionSuper);
    CCExpressionEvaluatorRegister(CC_STRING("strict-super"), CCStateExpressionStrictSuper);
    CCExpressionEvaluatorRegister(CC_STRING("prefix"), CCStringExpressionPrefix);
    CCExpressionEvaluatorRegister(CC_STRING("suffix"), CCStringExpressionSuffix);
    CCExpressionEvaluatorRegister(CC_STRING("filename"), CCStringExpressionFilename);
    CCExpressionEvaluatorRegister(CC_STRING("replace"), CCStringExpressionReplace);
    CCExpressionEvaluatorRegister(CC_STRING("window-percent-width"), CCWindowExpressionPercentageWidth);
    CCExpressionEvaluatorRegister(CC_STRING("window-percent-height"), CCWindowExpressionPercentageHeight);
    CCExpressionEvaluatorRegister(CC_STRING("window-width"), CCWindowExpressionWidth);
    CCExpressionEvaluatorRegister(CC_STRING("window-height"), CCWindowExpressionHeight);
}
