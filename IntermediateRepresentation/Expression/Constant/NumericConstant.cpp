////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <boost/lexical_cast.hpp>
#include <cmath>

#include "NumericConstant.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"

NumericConstant::NumericConstant(int _value) : Constant(NUMERIC_CONSTANT)
{
	value = _value;
	isSigned = true;
}

NumericConstant::NumericConstant(int _value, bool _isSigned) : Constant(NUMERIC_CONSTANT)
{
	value = _value;
	isSigned = _isSigned;
}

NumericConstant::~NumericConstant(void)
{
}

ExpressionPtr NumericConstant::deepcopy(){
	NumericConstantPtr copyPtr = std::make_shared<NumericConstant>(value, isSigned);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

bool NumericConstant::equals(ExpressionPtr toThisExpression){
	if(toThisExpression->type == NUMERIC_CONSTANT){
		return value == std::dynamic_pointer_cast<NumericConstant>(toThisExpression)->value;
	}
	return false;
}

std::string NumericConstant::getExpressionString(){
	if(value < 1 && value != 0 && value > -1){
		return boost::lexical_cast<std::string>(1/value) + "^(-1)";
	}
	if(isSigned)
		return boost::lexical_cast<std::string>(value);
	else
		return boost::lexical_cast<std::string>((unsigned int)value);
}

TypePtr NumericConstant::getExpresstionType(TypeMapPtr typeMap){
	if(std::abs(value) <= MIN_POINTER_VALUE/* && value != 0*/){
		return std::make_shared<IntegralType>(INT_TYPE);
	}
	return expressionType;
}

std::string NumericConstant::to_json(){
	if(isSigned)
		return "{\"expression_type\":\"NumericConstant\", \"value\":\"" + boost::lexical_cast<std::string>(value) + "\"}";
	else
		return "{\"expression_type\":\"NumericConstant\", \"value\":\"" + boost::lexical_cast<std::string>((unsigned int)value) + "\"}";
}
