////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "ExponentiationExpression.h"
#include "../Constant/NumericConstant.h"

#include "../../../TypeAnalysis/Types/TopType.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"
//#include "../../../TypeAnalysis/Types/SimpleType/PointerType.h"


ExponentiationExpression::ExponentiationExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand) : BinaryNonCommutativeExpression(EXPONENTIATION_EXPRESSION, _firstOperand, _secondOperand)
{
}

ExponentiationExpression::~ExponentiationExpression(void)
{
}

ExpressionPtr ExponentiationExpression::deepcopy(){
	ExponentiationExpressionPtr copyPtr = std::make_shared<ExponentiationExpression>(firstOperand->deepcopy(), secondOperand->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

ExpressionPtr ExponentiationExpression::simplifyExpression(){
	simplifyOperands();

	if(firstOperand->type == NUMERIC_CONSTANT){
		double baseConstantValue = std::dynamic_pointer_cast<NumericConstant>(firstOperand)->value;
		if(baseConstantValue == 0 || baseConstantValue == 1){
			NumericConstantPtr zeroOrOne = std::make_shared<NumericConstant>(baseConstantValue);
			return zeroOrOne;
		}
		else if(secondOperand->type == NUMERIC_CONSTANT){
			double exponentConstantValue = std::dynamic_pointer_cast<NumericConstant>(secondOperand)->value;
			NumericConstantPtr totalConstant = std::make_shared<NumericConstant>(std::pow(baseConstantValue, exponentConstantValue));
			return totalConstant;
		}
	}

	if(secondOperand->type == NUMERIC_CONSTANT){
		double exponentConstantValue = std::dynamic_pointer_cast<NumericConstant>(secondOperand)->value;
		if(exponentConstantValue == 0){
			NumericConstantPtr one = std::make_shared<NumericConstant>(1);
			return one;
		}
		else if(exponentConstantValue == 1){
			return firstOperand->deepcopy();
		}
	}
	return deepcopy();
}

std::string ExponentiationExpression::getOperationString(){
	return "^";
}

TypePtr ExponentiationExpression::getExpresstionType(TypeMapPtr typeMap){
	TypePtr firstOperandType = firstOperand->getExpresstionType(typeMap);
	TypePtr secondOperandType = secondOperand->getExpresstionType(typeMap);
	if(firstOperandType->type == INTEGRAL_TYPE && secondOperandType->type == INTEGRAL_TYPE){
		IntegralTypePtr firstOperandIntegralType = std::dynamic_pointer_cast<IntegralType>(firstOperandType);
		IntegralTypePtr secondOperandIntegralType = std::dynamic_pointer_cast<IntegralType>(secondOperandType);
		if(firstOperandIntegralType->integralType == INT_TYPE && secondOperandIntegralType->integralType == INT_TYPE)
			return std::make_shared<IntegralType>(INT_TYPE);
		else
			return std::make_shared<IntegralType>(DOUBLE_TYPE);
	}
	return std::make_shared<TopType>();
}

void ExponentiationExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){

}

std::string ExponentiationExpression::to_json(){
	std::string json_string = "{\"expression_type\":\"ExponentiationExpression\", ";
	json_string.append("\"firstOperand\":");
	json_string.append(firstOperand->to_json() + ", ");
	json_string.append("\"secondOperand\":");
	json_string.append(secondOperand->to_json() + "}");
	return json_string;
}
