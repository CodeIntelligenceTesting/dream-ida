////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include "localMathematicalSimplifications.h"
#include "SimplificationTypes.h"
#include "../../IntermediateRepresentation/Expression/Expression.h"
#include "../../IntermediateRepresentation/Expression/ExpressionTypes.h"
#include "../../IntermediateRepresentation/Expression/AssociativeCommutativeExpression/AdditionExpression.h"
#include "../../IntermediateRepresentation/Expression/AssociativeCommutativeExpression/MultiplicationExpression.h"
#include "../../IntermediateRepresentation/Expression/BinaryNonCommutativeExpression/ExponentiationExpression.h"
#include "../../IntermediateRepresentation/Expression/Constant/NumericConstant.h"
#include "../../IntermediateRepresentation/Expression/UnaryExpression/NegationExpression.h"


bool applyLocalSimplifications(ExpressionPtr& operandPtr, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	SimplificationType simplificationType = getSimplificationType(operandPtr, nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
	switch(simplificationType){
	case CONSTANT_NEGATION:
		{
			double  negatedVaule = -1 * std::dynamic_pointer_cast<NumericConstant>(replacingExpression)->value;
			operandPtr = std::make_shared<NumericConstant>(negatedVaule);
			return true;
		}
	case DOUBLE_LOGICAL_NOT:
	case DOUBLE_NEGATION:
		{
			operandPtr = std::dynamic_pointer_cast<UnaryExpression>(replacingExpression)->operand->deepcopy();
			return true;
		}
	case NEGATION_OF_ADDITION:
		{
			ExpressionPtr expressionAfterPropagation = replacingExpression->deepcopy();
			simplifyNegationOfAddition(expressionAfterPropagation);
			operandPtr = expressionAfterPropagation;
			return true;
		}
	case NEGATION_OF_MULTIPLICATION:
		{
			ExpressionPtr expressionAfterPropagation = replacingExpression->deepcopy();
			simplifyNegationOfMultiplication(expressionAfterPropagation);
			operandPtr = expressionAfterPropagation;
			return true;
		}
	case DOUBLE_EXPONENTIATION:
		{
			ExponentiationExpressionPtr outerExponentiation = std::dynamic_pointer_cast<ExponentiationExpression>(operandPtr);
			ExponentiationExpressionPtr innerExponentiation = std::dynamic_pointer_cast<ExponentiationExpression>(replacingExpression);
			ExpressionPtr expressionAfterPropagation = simplifyDoubleExponentiation(outerExponentiation, innerExponentiation);
			operandPtr = expressionAfterPropagation;
			return true;
		}
	case EXPONENTIATION_OF_MYLTIPLICATION:
		{
			MultiplicationExpressionPtr replacingMult = std::dynamic_pointer_cast<MultiplicationExpression>(replacingExpression);
			ExpressionPtr operandExponent = std::dynamic_pointer_cast<BinaryNonCommutativeExpression>(operandPtr)->secondOperand;
			ExpressionPtr expressionAfterPropagation = simplifyExponentiationOfMultiplication(replacingMult, operandExponent);
			operandPtr = expressionAfterPropagation;
			return true;
		}
	default:
		return false;
	}
}

void simplifyNegationOfAddition(ExpressionPtr& additionExpression){
	ExpressionVectorPtr operands = std::dynamic_pointer_cast<AdditionExpression>(additionExpression)->operands;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin(); op_iter != operands->end() ; ++op_iter){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NEGATION_EXPRESSION){
			NegationExpressionPtr operandNegationExpression = std::dynamic_pointer_cast<NegationExpression>(operandExpression);
			//if(operandUnaryExpression->operation.compare("-") == 0){
			ExpressionPtr newOperandExpression = operandNegationExpression->operand->deepcopy();
			*op_iter = newOperandExpression;
			//}
		}
		else if(operandExpression->type == NUMERIC_CONSTANT){
			std::dynamic_pointer_cast<NumericConstant>(operandExpression)->value *= -1;
		}
		else{
			*op_iter = std::make_shared<NegationExpression>(operandExpression);
			//*op_iter = new UnaryExpression("-", operandExpression);
		}
	}
}

void simplifyNegationOfMultiplication(ExpressionPtr& multiplicationExpression){
	bool constantValueNegated = false;
	ExpressionVectorPtr operands = std::dynamic_pointer_cast<MultiplicationExpression>(multiplicationExpression)->operands;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin(); op_iter != operands->end() ; ++op_iter){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NUMERIC_CONSTANT){
			std::dynamic_pointer_cast<NumericConstant>(operandExpression)->value *= -1;
			constantValueNegated = true;
			break;
		}
	}
	if(!constantValueNegated){
		NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
		operands->insert(operands->begin(), minusOne);
	}
}

ExpressionPtr simplifyDoubleExponentiation(const ExponentiationExpressionPtr& outerExponentiation, const ExponentiationExpressionPtr& innerExponentiation){
	ExpressionPtr outerExponent = outerExponentiation->secondOperand;
	ExpressionPtr innerExponent = innerExponentiation->secondOperand;

	ExpressionPtr exponent = multiplyAndSimplify(innerExponent, outerExponent);
	ExpressionPtr base = innerExponentiation->firstOperand->deepcopy();
	ExpressionPtr exponentiationExpression = std::make_shared<ExponentiationExpression>(base, exponent);
	
	ExpressionPtr simplifiedExponentiationExpression = exponentiationExpression->simplifyExpression();
	
	return simplifiedExponentiationExpression;
}

ExpressionPtr addAndSimplify(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression){
	ExpressionVectorPtr mergedOperands = std::make_shared<std::vector<ExpressionPtr>>();
	if(firstExpression->type == ADDITION_EXPRESSION){
		AdditionExpressionPtr firstAdditionExpression = std::dynamic_pointer_cast<AdditionExpression>(firstExpression);
		for(std::vector<ExpressionPtr>::iterator op_iter = firstAdditionExpression->operands->begin() ; op_iter != firstAdditionExpression->operands->end() ; ++op_iter){
			mergedOperands->push_back((*op_iter)->deepcopy());
		}
	}
	else{
		mergedOperands->push_back(firstExpression->deepcopy());
	}

	if(secondExpression->type == ADDITION_EXPRESSION){
		AdditionExpressionPtr secondAdditionExpression = std::dynamic_pointer_cast<AdditionExpression>(secondExpression);
		for(std::vector<ExpressionPtr>::iterator op_iter = secondAdditionExpression->operands->begin() ; op_iter != secondAdditionExpression->operands->end() ; ++op_iter){
			mergedOperands->push_back((*op_iter)->deepcopy());
		}
	}
	else{
		mergedOperands->push_back(secondExpression->deepcopy());
	}

	ExpressionPtr mergedExpression = std::make_shared<AdditionExpression>(mergedOperands);
	ExpressionPtr simplifiedExpression = mergedExpression->simplifyExpression();
	return simplifiedExpression;
}

ExpressionPtr multiplyAndSimplify(const ExpressionPtr &firstExpression, const ExpressionPtr &secondExpression){
	ExpressionVectorPtr mergedOperands = std::make_shared<std::vector<ExpressionPtr>>();
	
	if(firstExpression->type == MULTIPLICATION_EXPRESSION){
		MultiplicationExpressionPtr firstMultiplicationExpression = std::dynamic_pointer_cast<MultiplicationExpression>(firstExpression);
		for(std::vector<ExpressionPtr>::iterator op_iter = firstMultiplicationExpression->operands->begin() ; op_iter != firstMultiplicationExpression->operands->end() ; ++op_iter){
			mergedOperands->push_back((*op_iter)->deepcopy());
		}
	}
	else{
		mergedOperands->push_back(firstExpression->deepcopy());
	}

	if(secondExpression->type == MULTIPLICATION_EXPRESSION){
		MultiplicationExpressionPtr secondMultiplicationExpression = std::dynamic_pointer_cast<MultiplicationExpression>(secondExpression);
		for(std::vector<ExpressionPtr>::iterator op_iter = secondMultiplicationExpression->operands->begin() ; op_iter != secondMultiplicationExpression->operands->end() ; ++op_iter){
			mergedOperands->push_back((*op_iter)->deepcopy());
		}
	}
	else{
		mergedOperands->push_back(secondExpression->deepcopy());
	}

	ExpressionPtr mergedExpression = std::make_shared<MultiplicationExpression>(mergedOperands);
	ExpressionPtr simplifiedExpression = mergedExpression->simplifyExpression();
	return simplifiedExpression;
}

ExpressionPtr simplifyExponentiationOfMultiplication(const MultiplicationExpressionPtr& multiplicationExpression, const ExpressionPtr& exponent){
	//msg("simplifyExponentiationOfMultiplication(%s,%s)\n", multiplicationExpression->getExpressionString().c_str(), exponent->getExpressionString().c_str());
	ExpressionVectorPtr resultingOperands = std::make_shared<std::vector<ExpressionPtr>>();
	
	ExpressionVectorPtr multiplicationOperands = multiplicationExpression->operands;//((AssociativeCommutativeExpression*)multiplicationExpression)->operands;
	for(std::vector<ExpressionPtr>::iterator op_iter = multiplicationOperands->begin() ; op_iter != multiplicationOperands->end() ; ++op_iter){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == EXPONENTIATION_EXPRESSION){
			ExponentiationExpressionPtr operandExponentiationExpression = std::dynamic_pointer_cast<ExponentiationExpression>(operandExpression);
			ExpressionPtr operandBase = operandExponentiationExpression->firstOperand;
			ExpressionPtr operandExponent = operandExponentiationExpression->secondOperand;

			ExpressionPtr resultingExponent = multiplyAndSimplify(operandExponent, exponent);

			ExpressionPtr resultingExponentiationExpression = std::make_shared<ExponentiationExpression>(operandBase->deepcopy(), resultingExponent);
			ExpressionPtr simplifiedExpression = resultingExponentiationExpression->simplifyExpression();
			//Expression* resultingExponentiationExpression = getExponentiationExpression(operandBase, resultingExponent);
			resultingOperands->push_back(simplifiedExpression);
		}
		else{
			ExpressionPtr exponentiatedOperand = std::make_shared<ExponentiationExpression>(operandExpression->deepcopy(), exponent->deepcopy());
			resultingOperands->push_back(exponentiatedOperand);
		}
	}
	MultiplicationExpressionPtr result = std::make_shared<MultiplicationExpression>(resultingOperands);
	return result;
}
