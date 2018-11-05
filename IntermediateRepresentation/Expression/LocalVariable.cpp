////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "LocalVariable.h"


LocalVariable::LocalVariable(std::string _name) : Expression(LOCAL_VARIABLE, NO_SUBSCRIPT)
{
	name = _name;
	isSimpleExpression = true;
	//subscript = -1;
}


LocalVariable::~LocalVariable(void)
{
}


std::string LocalVariable::getExpressionString(){
	std::string subscriptString;
	if(subscript != -1){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
	}
	return name + subscriptString;// + ":(loc):" + expressionTypeToString();
}


char* LocalVariable::getColoredExpressionString(){
	return getColoredString(getExpressionString(), VARIABLE_COLOR);
}

std::string LocalVariable::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), VARIABLE_COLOR);
}

std::string LocalVariable::getName(){
	return "loc_" + name;
}

std::string LocalVariable::to_json(){
	return "{\"expression_type\":\"LocalVariable\", \"name\":\"" + getExpressionString() + "\", \"type\":\"" + expressionTypeToString() + "\"}";
}


//std::vector<Expression*>* LocalVariable::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->push_back(this);
//	return expElements;
//}


ExpressionPtr LocalVariable::deepcopy(){
	LocalVariablePtr copyPtr = std::make_shared<LocalVariable>(name);
	copyPtr->subscript = subscript;
	copyPtr->phiSourceBlockId = phiSourceBlockId;
	copyPtr->expressionType = expressionType->deepcopy();
	copyPtr->set_expression_size(size_in_bytes);
	return copyPtr;
}
