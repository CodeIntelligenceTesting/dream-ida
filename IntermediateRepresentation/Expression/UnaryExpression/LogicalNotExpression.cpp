////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "LogicalNotExpression.h"
#include "../AssociativeCommutativeExpression/AdditionExpression.h"
#include "../Constant/NumericConstant.h"
#include "../../../DataFlowAnalysis/MathematicalSimplification/localMathematicalSimplifications.h"

LogicalNotExpression::LogicalNotExpression(ExpressionPtr _operand, bool _isConditionalExpression) : UnaryExpression(LOGICAL_NOT_EXPRESSION, _operand)
{
	isConditionalExpression = _isConditionalExpression;
}


LogicalNotExpression::~LogicalNotExpression(void)
{
}

ExpressionPtr LogicalNotExpression::deepcopy(){
	LogicalNotExpressionPtr copyPtr = std::make_shared<LogicalNotExpression>(operand->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

ExpressionPtr LogicalNotExpression::simplifyExpression(){
	if(operand->type == ADDITION_EXPRESSION){
		simplifyNegationOfAddition(operand);
		NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
		std::dynamic_pointer_cast<AdditionExpression>(operand)->operands->push_back(minusOne);
		return operand->simplifyExpression();
	}
	ExpressionPtr nullPtr = std::shared_ptr<Expression>();;
	return nullPtr;
}

std::string LogicalNotExpression::getOperationString(){
	return isConditionalExpression ? "!" : "~";
}

std::string LogicalNotExpression::to_json(){
	return "{\"expression_type\":\"LogicalNotExpression\", \"operand\":" + operand->to_json() + "}";
}
