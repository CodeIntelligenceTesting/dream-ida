/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"

bool applyLocalSimplifications(ExpressionPtr& operandPtr, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

void simplifyNegationOfAddition(ExpressionPtr& additionExpression);
void simplifyNegationOfMultiplication(ExpressionPtr& multiplicationExpression);
ExpressionPtr simplifyDoubleExponentiation(const ExponentiationExpressionPtr& outerExponentiation, const ExponentiationExpressionPtr& innerExponentiation);
ExpressionPtr addAndSimplify(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression);
ExpressionPtr multiplyAndSimplify(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression);
//Expression* getExponentiationExpression(Expression* base, Expression* exponent);
ExpressionPtr simplifyExponentiationOfMultiplication(const MultiplicationExpressionPtr& multiplicationExpression, const ExpressionPtr& exponent);
