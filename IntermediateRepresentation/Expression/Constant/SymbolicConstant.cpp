////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "SymbolicConstant.h"

#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"

SymbolicConstant::SymbolicConstant(SymbolicConstantValue _value) : Constant(SYMBOLIC_CONSTANT)
{
	value = _value;
}


SymbolicConstant::~SymbolicConstant(void)
{
}

ExpressionPtr SymbolicConstant::deepcopy(){
	SymbolicConstantPtr copyPtr = std::make_shared<SymbolicConstant>(value);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

bool SymbolicConstant::equals(ExpressionPtr toThisExpression){
	if(toThisExpression->type == SYMBOLIC_CONSTANT){
		return value == std::dynamic_pointer_cast<SymbolicConstant>(toThisExpression)->value;
	}
	return false;
}


std::string SymbolicConstant::getExpressionString(){
	switch(value){
	case PI:
		return "PI";
	case E:
		return "e";
	}
	msg("unokwn SymbolicConstant.\n");
	return "unokwn SymbolicConstant";
}

TypePtr SymbolicConstant::getExpresstionType(TypeMapPtr typeMap){
	return std::make_shared<IntegralType>(DOUBLE_TYPE);
}

std::string SymbolicConstant::to_json(){
	return "{\"expression_type\":\"SymbolicConstant\", \"value\":\"" + getExpressionString() + "\"}";
}
