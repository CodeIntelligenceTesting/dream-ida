////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ExpressionUtilities.h"
#include "../IntermediateRepresentation/Expression/Expression.h"
#include "../IntermediateRepresentation/Expression/ExpressionTypes.h"
#include "../IntermediateRepresentation/Expression/AssociativeCommutativeExpression/ANDExpression.h"
#include "../IntermediateRepresentation/Expression/AssociativeCommutativeExpression/ORExpression.h"
#include "../IntermediateRepresentation/Expression/Condition/HighLevelCondition.h"
#include "../IntermediateRepresentation/Expression/UnaryExpression/LogicalNotExpression.h"


void createExpressionElements(const ExpressionPtr& rootExpressionPtr, bool includePointers, std::vector<ExpressionPtr>& elements_out){
	if(!isExpressionPointerNull(rootExpressionPtr)){
		addRootElement(rootExpressionPtr, includePointers, elements_out);
		rootExpressionPtr->getExpressionElements(includePointers, elements_out);
	}
}

void addRootElement(const ExpressionPtr& rootExpressionPtr, bool includePointers, std::vector<ExpressionPtr>& elements_out){
	addElementIfSimple(rootExpressionPtr, elements_out);
	if(includePointers)
		addElementIfMemoryVariable(rootExpressionPtr, elements_out);
}

void addElementIfSimple(const ExpressionPtr& rootExpressionPtr, std::vector<ExpressionPtr>& elements_out){
	if(rootExpressionPtr->isSimpleExpression)
		elements_out.push_back(rootExpressionPtr);
}

void addElementIfMemoryVariable(const ExpressionPtr& rootExpressionPtr, std::vector<ExpressionPtr>& elements_out){
	if(rootExpressionPtr->type == POINTER)
		elements_out.push_back(rootExpressionPtr);
}

bool isExpressionPointerNull(const ExpressionPtr& expPtr){
	return expPtr == std::shared_ptr<Expression>();
}


ExpressionPtr getNegatedExpression(ExpressionPtr &booleanExpression){
	switch(booleanExpression->type){
	case LOGICAL_NOT_EXPRESSION:
		{
			return std::dynamic_pointer_cast<LogicalNotExpression>(booleanExpression)->operand;
		}
	case LOWLEVEL_CONDITION:
		{
			LogicalNotExpressionPtr negatedExpression = std::make_shared<LogicalNotExpression>(booleanExpression, true);
			return negatedExpression;
		}
	case HIGHLEVEL_CONDITION:
		{
			HighLevelConditionPtr highLevelCondition = std::dynamic_pointer_cast<HighLevelCondition>(booleanExpression);
			highLevelCondition->comparisonOperand = getNegatedOperator(highLevelCondition->comparisonOperand);
			return highLevelCondition;
		}
	case AND_EXPRESSION:
		{
			ExpressionVectorPtr ORoperands = std::make_shared<std::vector<ExpressionPtr>>();
			ANDExpressionPtr andExpression = std::dynamic_pointer_cast<ANDExpression>(booleanExpression);
			for(std::vector<ExpressionPtr>::iterator op_it = andExpression->operands->begin() ; op_it != andExpression->operands->end() ; ++op_it){
				ORoperands->push_back(getNegatedExpression(*op_it));
			}
			ORExpressionPtr negatedExpression = std::make_shared<ORExpression>(ORoperands, true);
			return negatedExpression;
		}
	case OR_EXPRESSION:
		{
			ExpressionVectorPtr ANDoperands = std::make_shared<std::vector<ExpressionPtr>>();
			ORExpressionPtr orExpression = std::dynamic_pointer_cast<ORExpression>(booleanExpression);
			for(std::vector<ExpressionPtr>::iterator op_it = orExpression->operands->begin() ; op_it != orExpression->operands->end() ; ++op_it){
				ANDoperands->push_back(getNegatedExpression(*op_it));
			}
			ANDExpressionPtr negatedExpression = std::make_shared<ANDExpression>(ANDoperands, true);
			return negatedExpression;
		}
	}
}

ANDExpressionPtr getCompundCondition_AND(ExpressionPtr &conditionA, ExpressionPtr &conditionB){
	ANDExpressionPtr result = std::shared_ptr<ANDExpression>();
	if(conditionA->type != AND_EXPRESSION && conditionB->type != AND_EXPRESSION){
		ExpressionVectorPtr compoundConditionOperands = std::make_shared<std::vector<ExpressionPtr>>();
		compoundConditionOperands->push_back(conditionA);
		compoundConditionOperands->push_back(conditionB);
		ANDExpressionPtr compoundCondition = std::make_shared<ANDExpression>(compoundConditionOperands, true);
		result = compoundCondition;
	}
	else if(conditionA->type == AND_EXPRESSION){
		std::dynamic_pointer_cast<ANDExpression>(conditionA)->operands->push_back(conditionB);
	}
	else if(conditionB->type == AND_EXPRESSION){
		ANDExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ANDExpression>(conditionB);
		compoundConditionB->operands->push_back(conditionA);
		result = compoundConditionB;
	}
	else{
		ANDExpressionPtr compoundConditionA = std::dynamic_pointer_cast<ANDExpression>(conditionA);
		ANDExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ANDExpression>(conditionB);
		for(std::vector<ExpressionPtr>::iterator op_it = compoundConditionB->operands->begin() ; op_it != compoundConditionB->operands->end() ; ++op_it){
			compoundConditionA->operands->push_back(*op_it);
		}
	}
	return result;
}

ORExpressionPtr getCompundCondition_OR(ExpressionPtr &conditionA, ExpressionPtr &conditionB){
	ORExpressionPtr result = std::shared_ptr<ORExpression>();
	if(conditionA->type != OR_EXPRESSION && conditionB->type != OR_EXPRESSION){
		ExpressionVectorPtr compoundConditionOperands = std::make_shared<std::vector<ExpressionPtr>>();
		compoundConditionOperands->push_back(conditionA);
		compoundConditionOperands->push_back(conditionB);
		ORExpressionPtr compoundCondition = std::make_shared<ORExpression>(compoundConditionOperands, true);
		result = compoundCondition;
	}
	else if(conditionA->type == OR_EXPRESSION){
		std::dynamic_pointer_cast<ORExpression>(conditionA)->operands->push_back(conditionB);
	}
	else if(conditionB->type == OR_EXPRESSION){
		ORExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ORExpression>(conditionB);
		compoundConditionB->operands->push_back(conditionA);
		result = compoundConditionB;
	}
	else{
		ORExpressionPtr compoundConditionA = std::dynamic_pointer_cast<ORExpression>(conditionA);
		ORExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ORExpression>(conditionB);
		for(std::vector<ExpressionPtr>::iterator op_it = compoundConditionB->operands->begin() ; op_it != compoundConditionB->operands->end() ; ++op_it){
			compoundConditionA->operands->push_back(*op_it);
		}
	}
	return result;
}

std::string getNegatedOperator(std::string conditionalOperator){
	return conditionalOperator.compare("!=") == 0 ? "=="
		: conditionalOperator.compare("==") == 0 ? "!="
		: conditionalOperator.compare(">") == 0 ? "<="
		: conditionalOperator.compare(">=") == 0 ? "<"
		: conditionalOperator.compare("<") == 0 ? ">="
		: conditionalOperator.compare("<=") == 0 ? ">"
		: "unknownNegatedOperator";
}
