////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ANDExpression.h"
#include "../Constant/NumericConstant.h"
#include "../UnaryExpression/LogicalNotExpression.h"

#include "../../../TypeAnalysis/Types/TopType.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../../TypeAnalysis/Types/SimpleType/PointerType.h"


ANDExpression::ANDExpression(ExpressionVectorPtr _operands, bool _isConditionalExpression) : AssociativeCommutativeExpression(_operands, AND_EXPRESSION)
{
	isConditionalExpression = _isConditionalExpression;
}

ANDExpression::~ANDExpression(void)
{
}

ExpressionPtr ANDExpression::deepcopy(){
	ExpressionVectorPtr operandsCopy = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		operandsCopy->push_back((*op_iter)->deepcopy());
	}
	ANDExpressionPtr copyPtr = std::make_shared<ANDExpression>(operandsCopy);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

ExpressionPtr ANDExpression::simplifyExpression(){
	simplifyOperands();
	for(std::vector<ExpressionPtr>::iterator currentOp_iter = operands->begin() ; currentOp_iter != operands->end() ; ++currentOp_iter){
		ExpressionPtr currentOperand = *currentOp_iter;
		std::vector<ExpressionPtr>::iterator nextOp_iter = currentOp_iter + 1;
		while(nextOp_iter != operands->end()){
			ExpressionPtr nextOperand = *nextOp_iter;
			MergeType logicalMergeType = getMergeType(currentOperand, nextOperand);
			switch(logicalMergeType){
			case TWO_NUMERIC_CONSTANTS:
				{
					NumericConstantPtr currentNumericConstant = std::dynamic_pointer_cast<NumericConstant>(currentOperand);
					NumericConstantPtr nextNumericConstant = std::dynamic_pointer_cast<NumericConstant>(nextOperand);
					currentNumericConstant->value = ((long)currentNumericConstant->value) & ((long)nextNumericConstant->value);
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			//case TWO_SYMBOLIC_CONSTANTS:
			case TWO_EQUAL_EXPRESSIONS:
				{
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case LOGICAL_EXPRESSION_WITH_ITS_NEGATION:
				{
					NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
					return zero;
				}
				//return new NumericLiteral(0);
			default:
				++nextOp_iter;
			}
		}
	}
	ExpressionPtr simplifiedExpression = combineConstants();
	return simplifiedExpression;
}

std::string ANDExpression::getOperationString(){
	return isConditionalExpression ? "&&" : "&";
}

TypePtr ANDExpression::getExpresstionType(TypeMapPtr typeMap){
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		TypePtr opType = (*op_iter)->getExpresstionType(typeMap);
		if(opType->type == POINTER_TYPE){
			return std::make_shared<PointerType>();
		}
		else{
			return std::make_shared<TopType>();
		}
	}
	return std::make_shared<IntegralType>(INT_TYPE);
}

void ANDExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
	if(type->isIntegerType()){
		for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
			(*op_iter)->updateExpressionType(type, typeMap);
		}
	}
	else if(type->isPointerType() && doesHavePointerOperand(typeMap)){
		IntegralTypePtr intType = std::make_shared<IntegralType>(INT_TYPE);
		for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
			if(!(*op_iter)->getExpresstionType(typeMap)->isPointerType())
				(*op_iter)->updateExpressionType(intType, typeMap);
		}
	}
}

ExpressionPtr ANDExpression::combineConstants(){
	long totalConstantValue = -1;
	std::vector<ExpressionPtr>::iterator op_iter = operands->begin();
	while(op_iter != operands->end()){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NUMERIC_CONSTANT){
			totalConstantValue &= (long)(std::dynamic_pointer_cast<NumericConstant>(operandExpression)->value);
			op_iter = operands->erase(op_iter);
		}
		else{
			++op_iter;
		}
	}

	if(totalConstantValue == 0){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		return zero;
	}
	else if(operands->size() == 0){
		NumericConstantPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue, false);
		return totalConstant;
	}
	else if(operands->size() == 1 && totalConstantValue == -1){
		return (*(operands->begin()))->deepcopy();
	}
	else{
		if(totalConstantValue != -1){
			NumericConstantPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue, false);
			operands->insert(operands->begin(), totalConstant);
			//operands->insert(operands->begin(), new NumericLiteral(totalConstantValue));
		}
		return deepcopy();
	}
}

MergeType ANDExpression::getMergeType(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression){
	//MergeType constantMergeType = getConstantsMergeType(firstExpression, secondExpression);
	if(firstExpression->type == NUMERIC_CONSTANT && secondExpression->type == NUMERIC_CONSTANT){
		return TWO_NUMERIC_CONSTANTS;
	}
	else if(firstExpression->equals(secondExpression)){
		return TWO_EQUAL_EXPRESSIONS;
	}
	else if(firstExpression->type == LOGICAL_NOT_EXPRESSION){
		LogicalNotExpressionPtr firstLogicalNotExpression = std::dynamic_pointer_cast<LogicalNotExpression>(firstExpression);
		if(firstLogicalNotExpression->operand->equals(secondExpression)){
			return LOGICAL_EXPRESSION_WITH_ITS_NEGATION;
		}
	}
	else if(secondExpression->type == LOGICAL_NOT_EXPRESSION){
		LogicalNotExpressionPtr secondLogicalNotExpression = std::dynamic_pointer_cast<LogicalNotExpression>(secondExpression);
		if(secondLogicalNotExpression->operand->equals(firstExpression)){
			return LOGICAL_EXPRESSION_WITH_ITS_NEGATION;
		}
	}
	return NO_MERGE_POSSIBLE;
}
