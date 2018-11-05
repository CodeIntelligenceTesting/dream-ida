/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once


#include "SimplificationTypes.h"
#include "../../IntermediateRepresentation/IRSharedPointers.h"

SimplificationType getSimplificationType(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
bool doesPropagationResultsInConstantNegation(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
//bool doesPropagationResultsInDoubleUnaryExpression(Expression* operandExpression, std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression);
bool doesPropagationResultsInDoubleNegation(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
bool doesPropagationResultsInDoubleLogicalNOT(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
bool doesPropagationResultsInNegationOfAdditionOrMultiplication(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
//bool doesPropagationResultsInMultiplicationWithDivision(Expression* operandExpression, std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression);
bool doesPropagationResultsInDoubleExponentiation(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
bool doesPropagationResultsInExponentiationOfMultiplication(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

//SimplificationType getPotentialDoubleDivisionSimplificationType(Expression* operandExpression, std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression);
