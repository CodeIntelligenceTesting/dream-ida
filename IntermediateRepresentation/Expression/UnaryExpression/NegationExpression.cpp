////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "NegationExpression.h"


NegationExpression::NegationExpression(ExpressionPtr _operand) : UnaryExpression(NEGATION_EXPRESSION, _operand)
{
}


NegationExpression::~NegationExpression(void)
{
}

ExpressionPtr NegationExpression::deepcopy(){
	NegationExpressionPtr copyPtr = std::make_shared<NegationExpression>(operand->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

//ExpressionPtr NegationExpression::simplifyExpression(){
//	return NULL;
//}

std::string NegationExpression::getOperationString(){
	return "-";
}

std::string NegationExpression::to_json(){
	return "{\"expression_type\":\"NegationExpression\", \"operand\":" + operand->to_json() + "}";
}
