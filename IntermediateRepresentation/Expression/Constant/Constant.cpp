////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Constant.h"


Constant::Constant(ExpressionType _expressionType) : Expression(_expressionType, NO_SUBSCRIPT)
{
	isSimpleExpression = true;
}


Constant::~Constant(void)
{
}

//std::vector<Expression*>* Constant::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* elements = new std::vector<Expression*>();
//	elements->push_back(this);
//	return elements;
//}

char* Constant::getColoredExpressionString(){
	return getColoredString(getExpressionString(), NUMBER_COLOR);
}

std::string Constant::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), NUMBER_COLOR);
}
