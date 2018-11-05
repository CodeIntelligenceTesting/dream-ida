////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "StackVariable.h"


StackVariable::StackVariable(std::string _name) : Location(LOCAL_VARIABLE, _name, NO_SUBSCRIPT)
{
	isSimpleExpression = true;
}


StackVariable::~StackVariable(void)
{
}


std::string StackVariable::getExpressionString(){
	std::string subscriptString;
	if(subscript != -1){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
	}
	return name + subscriptString;
}


char* StackVariable::getColoredExpressionString(){
	return getColoredString(getExpressionString(), VARIABLE_COLOR);
}


std::string StackVariable::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), VARIABLE_COLOR);
}

std::string StackVariable::to_json(){
	//return "{\"expression_type\":\"StackVariable\", \"name\":\"" + getExpressionString() + "\"}";
	return "{\"expression_type\":\"LocalVariable\", \"name\":\"" + getExpressionString() + "\"}";
}

std::string StackVariable::getName(){
	return "stk_" + name;
}


//std::vector<Expression*>* StackVariable::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->push_back(this);
//	return expElements;
//}


ExpressionPtr StackVariable::deepcopy(){
	StackVariablePtr copyPtr = std::make_shared<StackVariable>(name);
	copyPtr->subscript = subscript;
	copyPtr->phiSourceBlockId = phiSourceBlockId;
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}
