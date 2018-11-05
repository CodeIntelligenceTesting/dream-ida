////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DivisionExpression.h"

#include "../../../TypeAnalysis/Types/TopType.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"

DivisionExpression::DivisionExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand) : BinaryNonCommutativeExpression(DIVISION_EXPRESSION, _firstOperand, _secondOperand)
{
}


DivisionExpression::~DivisionExpression(void)
{
}

ExpressionPtr DivisionExpression::deepcopy(){
	DivisionExpressionPtr copyPtr = std::make_shared<DivisionExpression>(firstOperand->deepcopy(), secondOperand->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

std::string DivisionExpression::getOperationString(){
	return "/";
}

TypePtr DivisionExpression::getExpresstionType(TypeMapPtr typeMap){
	return std::make_shared<IntegralType>(INT_TYPE);
	/*TypePtr firstOperandType = firstOperand->getExpresstionType(typeMap);
	TypePtr secondOperandType = secondOperand->getExpresstionType(typeMap);
	if(firstOperandType->type == INTEGRAL_TYPE && secondOperandType->type == INTEGRAL_TYPE){
		IntegralTypePtr firstOperandIntegralType = std::dynamic_pointer_cast<IntegralType>(firstOperandType);
		IntegralTypePtr secondOperandIntegralType = std::dynamic_pointer_cast<IntegralType>(secondOperandType);
		if(firstOperandIntegralType->integralType == INT_TYPE && secondOperandIntegralType->integralType == INT_TYPE)
			return std::make_shared<IntegralType>(INT_TYPE);
		else
			return std::make_shared<IntegralType>(DOUBLE_TYPE);
	}
	return std::make_shared<TopType>();*/
}

void DivisionExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
	/*if(type->type == INTEGRAL_TYPE){
		IntegralTypePtr resultType = std::dynamic_pointer_cast<IntegralType>(type);
		if(resultType->integralType == DOUBLE_TYPE){
			
		}
		else if(resultType->integralType == INT_TYPE){
			
		}
		TypePtr firstOperandType = firstOperand->getExpresstionType(typeMap);
		TypePtr secondOperandType = secondOperand->getExpresstionType(typeMap);
	}*/
}

std::string DivisionExpression::to_json(){
	std::string json_string = "{\"expression_type\":\"DivisionExpression\", ";
	json_string.append("\"firstOperand\":");
	json_string.append(firstOperand->to_json() + ", ");
	json_string.append("\"secondOperand\":");
	json_string.append(secondOperand->to_json() + "}");
	return json_string;
}
