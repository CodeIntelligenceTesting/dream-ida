////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ORExpression.h"
#include "../Constant/NumericConstant.h"
#include "../UnaryExpression/LogicalNotExpression.h"

#include "../../../TypeAnalysis/Types/TopType.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../../TypeAnalysis/Types/SimpleType/PointerType.h"


ORExpression::ORExpression(ExpressionVectorPtr _operands, bool _isConditionalExpression) : AssociativeCommutativeExpression(_operands, OR_EXPRESSION)
{
	isConditionalExpression = _isConditionalExpression;
}

ORExpression::~ORExpression(void)
{
}

ExpressionPtr ORExpression::deepcopy(){
	ExpressionVectorPtr operandsCopy = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		operandsCopy->push_back((*op_iter)->deepcopy());
	}
	ExpressionPtr copyPtr = std::make_shared<ORExpression>(operandsCopy);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

ExpressionPtr ORExpression::simplifyExpression(){
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
					currentNumericConstant->value = ((long)currentNumericConstant->value) | ((long)nextNumericConstant->value);
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
					NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
					return minusOne;
				}
				//return new NumericLiteral(-1);
			default:
				++nextOp_iter;
			}
		}
	}
	ExpressionPtr simplifiedExpression = combineConstants();
	return simplifiedExpression;
}

std::string ORExpression::getOperationString(){
	return isConditionalExpression ? "||" :"|";
}

TypePtr ORExpression::getExpresstionType(TypeMapPtr typeMap){
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

ExpressionPtr ORExpression::combineConstants(){
	long totalConstantValue = 0;
	std::vector<ExpressionPtr>::iterator op_iter = operands->begin();
	while(op_iter != operands->end()){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NUMERIC_CONSTANT){
			totalConstantValue |= (long)(std::dynamic_pointer_cast<NumericConstant>(operandExpression)->value);
			op_iter = operands->erase(op_iter);
		}
		else{
			++op_iter;
		}
	}

	if(operands->size() == 0){
		ExpressionPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue, false);
		return totalConstant;
		//return new NumericLiteral(totalConstantValue);
	}
	else if(totalConstantValue == 0){
		if(operands->size() == 1){
			return (*(operands->begin()))->deepcopy();
		}
		else{
			return deepcopy();
		}
	}
	else{
		ExpressionPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue, false);
		operands->push_back(totalConstant);
		//operands->push_back(new NumericLiteral(totalConstantValue));
		return deepcopy();
	}
}

MergeType ORExpression::getMergeType(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression){
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

void ORExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
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
