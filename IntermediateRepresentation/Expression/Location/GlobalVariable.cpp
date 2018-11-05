////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "GlobalVariable.h"
#include <boost/lexical_cast.hpp>


GlobalVariable::GlobalVariable(std::string _name, ea_t _address) : Location(GLOBAL_VARIABLE, _name, NO_SUBSCRIPT)
{
	address = _address;
	isSimpleExpression = true;
}


GlobalVariable::~GlobalVariable(void)
{
}


std::string GlobalVariable::getExpressionString(){
	std::string subscriptString;
	if(subscript != -1){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
	}
	return name + subscriptString;
}


char* GlobalVariable::getColoredExpressionString(){
	return getColoredString(getExpressionString(), VARIABLE_COLOR);
}


std::string GlobalVariable::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), VARIABLE_COLOR);
}

std::string GlobalVariable::to_json(){
	return "{\"expression_type\":\"GlobalVariable\", \"name\":\"" + name + "\", \"address\":\"" + boost::lexical_cast<std::string>(address) + "\"}";
}

std::string GlobalVariable::getName(){
	return "glb_" + name;
}


//std::vector<Expression*>* GlobalVariable::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->push_back(this);
//	return expElements;
//}


ExpressionPtr GlobalVariable::deepcopy(){
	GlobalVariablePtr copyPtr = std::make_shared<GlobalVariable>(name, address);
	copyPtr->subscript = subscript;
	copyPtr->phiSourceBlockId = phiSourceBlockId;
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}


//bool GlobalVariable::equals(Expression* toThisExpression){
//	if(type == toThisExpression->type){
//		return toThisExpression->getName().compare(getName()) == 0 && toThisExpression->simplifyExpression == subscript;
//	}
//	return false;
//}
//
//
//bool GlobalVariable::equalsDiscardConstant(Expression* toThisExpression){
//	return equals(toThisExpression);
//}
