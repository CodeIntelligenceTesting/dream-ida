////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "MultiplicationExpression.h"
#include "AdditionExpression.h"
#include "../BinaryNonCommutativeExpression/ExponentiationExpression.h"
#include "../UnaryExpression/NegationExpression.h"
#include "../Constant/NumericConstant.h"

#include "../../../TypeAnalysis/Types/TopType.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"


MultiplicationExpression::MultiplicationExpression(ExpressionVectorPtr _operands) : AssociativeCommutativeExpression(_operands, MULTIPLICATION_EXPRESSION)
{
}

MultiplicationExpression::~MultiplicationExpression(void)
{
}

ExpressionPtr MultiplicationExpression::deepcopy(){
	ExpressionVectorPtr operandsCopy = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		operandsCopy->push_back((*op_iter)->deepcopy());
	}
	MultiplicationExpressionPtr copyPtr = std::make_shared<MultiplicationExpression>((operandsCopy));
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

ExpressionPtr MultiplicationExpression::simplifyExpression(){
	removeNegatedOperands();
	simplifyOperands();
	for(std::vector<ExpressionPtr>::iterator currentOp_iter = operands->begin() ; currentOp_iter != operands->end() ; ++currentOp_iter){
		ExpressionPtr currentOperand = *currentOp_iter;
		std::vector<ExpressionPtr>::iterator nextOp_iter = currentOp_iter + 1;
		while(nextOp_iter != operands->end()){
			ExpressionPtr nextOperand = *nextOp_iter;
			MergeType multiplicationMergeType = getMergeType(currentOperand, nextOperand);
			//msg("%s, %s, %d\n", currentOperand->getExpressionString().c_str(), nextOperand->getExpressionString().c_str(), multiplicationMergeType);
			switch(multiplicationMergeType){
			case TWO_NUMERIC_CONSTANTS:
				{
					NumericConstantPtr currentOperandPtr = std::dynamic_pointer_cast<NumericConstant>(currentOperand);
					NumericConstantPtr nextOperandPtr = std::dynamic_pointer_cast<NumericConstant>(nextOperand);
					currentOperandPtr->value *= nextOperandPtr->value;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			//case TWO_SYMBOLIC_CONSTANTS:
			case TWO_EQUAL_EXPRESSIONS:
				{
					NumericConstantPtr two = std::make_shared<NumericConstant>(2);
					
					ExpressionPtr mergedExpression = std::make_shared<ExponentiationExpression>(currentOperand->deepcopy(), two);
					*currentOp_iter = mergedExpression;
					currentOperand = *currentOp_iter;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case MULTIPLICATION_EXPRESSION_WITH_EXPONENTIATION_EXPRESSION:
				{
					ExpressionPtr mergedExpression;
					if(currentOperand->type == EXPONENTIATION_EXPRESSION){
						ExponentiationExpressionPtr currentOpExp = std::dynamic_pointer_cast<ExponentiationExpression>(currentOperand);
						mergedExpression = simplifyExponentiationExpressionAfterAddingOneToExponent(currentOpExp);
					}
					else{
						ExponentiationExpressionPtr nextOpExp = std::dynamic_pointer_cast<ExponentiationExpression>(nextOperand);
						mergedExpression = simplifyExponentiationExpressionAfterAddingOneToExponent(nextOpExp);
					}
					*currentOp_iter = mergedExpression;
					currentOperand = *currentOp_iter;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case MULTIPLICATION_TWO_EXPONENTIATION_EXPRESSIONS:
				{
					ExponentiationExpressionPtr currentOpExp = std::dynamic_pointer_cast<ExponentiationExpression>(currentOperand);
					ExponentiationExpressionPtr nextOpExp = std::dynamic_pointer_cast<ExponentiationExpression>(nextOperand);
					ExpressionPtr mergedExpression = simplifyMultiplicationOfTwoExponentiationExpressions(currentOpExp, nextOpExp);
					*currentOp_iter = mergedExpression;
					currentOperand = *currentOp_iter;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case NO_MERGE_POSSIBLE:
				++nextOp_iter;
			}
		}
	}
	ExpressionPtr simplifiedExpression = combineConstants();
	return simplifiedExpression;
}

std::string MultiplicationExpression::getOperationString(){
	return "*";
}

TypePtr MultiplicationExpression::getExpresstionType(TypeMapPtr typeMap){
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		TypePtr opType = (*op_iter)->getExpresstionType(typeMap);
		if(opType->type == INTEGRAL_TYPE){
			IntegralTypePtr opIntegralType = std::dynamic_pointer_cast<IntegralType>(opType);
			if(opIntegralType->integralType == DOUBLE_TYPE){
				return std::make_shared<IntegralType>(DOUBLE_TYPE);
			}
		}
		else{
			return std::make_shared<TopType>();
		}
	}
	return std::make_shared<IntegralType>(INT_TYPE);
}

void MultiplicationExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
	if(type->isIntegerType() || type->isFloatType()){
		for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
			(*op_iter)->updateExpressionType(type, typeMap);
		}
	}
}

ExpressionPtr MultiplicationExpression::simplifyExponentiationExpressionAfterAddingOneToExponent(const ExponentiationExpressionPtr& exponentiationExpression){
	//BinaryNonCommutativeExpression* exponentiationBinaryExpression = (BinaryNonCommutativeExpression*)exponentiationExpression;
	ExpressionPtr exponentExpression = exponentiationExpression->secondOperand;
	ExpressionPtr simplifiedExponentExpression;
	//bool exponentIsAdditionExpression = false;
	if(exponentExpression->type == ADDITION_EXPRESSION){
		AssociativeCommutativeExpressionPtr exponentAssCommExp = std::dynamic_pointer_cast<AdditionExpression>(exponentExpression);
		//if(exponentAssCommExp->operation.compare("+") == 0){
		NumericConstantPtr one = std::make_shared<NumericConstant>(1);
		exponentAssCommExp->operands->push_back(one);
		//exponentAssCommExp->operands->push_back(new NumericLiteral(1));
		simplifiedExponentExpression = exponentExpression->simplifyExpression();
		//exponentIsAdditionExpression = true;
		//}
	}
	else{
	//if(!exponentIsAdditionExpression){
		ExpressionVectorPtr exponentOperands = std::make_shared<std::vector<ExpressionPtr>>();

		exponentOperands->push_back(exponentExpression->deepcopy());
		NumericConstantPtr one = std::make_shared<NumericConstant>(1);
		exponentOperands->push_back(one);
		exponentExpression = std::make_shared<AdditionExpression>(exponentOperands);
		simplifiedExponentExpression = exponentExpression->simplifyExpression();
	}

	if(simplifiedExponentExpression->type == NUMERIC_CONSTANT){
		double exponentConstantValue = std::dynamic_pointer_cast<NumericConstant>(simplifiedExponentExpression)->value;
		if(exponentConstantValue == 0){
			NumericConstantPtr one = std::make_shared<NumericConstant>(1);
			return one;
		}
		else if(exponentConstantValue == 1){
			return exponentiationExpression->firstOperand->deepcopy();
		}
	}
	ExpressionPtr baseCopy = exponentiationExpression->firstOperand->deepcopy();
	ExponentiationExpressionPtr result = std::make_shared<ExponentiationExpression>(baseCopy, simplifiedExponentExpression);
	return result;
	//return new BinaryNonCommutativeExpression("^", exponentiationBinaryExpression->firstOperand->deepcopy(), simplifiedExponentExpression);
}
	
ExpressionPtr MultiplicationExpression::simplifyMultiplicationOfTwoExponentiationExpressions(const ExponentiationExpressionPtr& firstExponentiationExpression, const ExponentiationExpressionPtr& secondExponentiationExpression){
	//BinaryNonCommutativeExpression* firstExponentiationBinaryExpression = (BinaryNonCommutativeExpression*)firstExponentiationExpression;
	//BinaryNonCommutativeExpression* secondExponentiationBinaryExpression = (BinaryNonCommutativeExpression*)secondExponentiationExpression;
	
	ExpressionPtr firstExponent = firstExponentiationExpression->secondOperand;
	ExpressionPtr secondExponent = secondExponentiationExpression->secondOperand;

	ExpressionPtr exponent = addAndSimplify(firstExponent, secondExponent);
	ExpressionPtr base = firstExponentiationExpression->firstOperand->deepcopy();
	
	ExpressionPtr exponentiationExpression = std::make_shared<ExponentiationExpression>(base, exponent);
	ExpressionPtr simplifiedExpression = exponentiationExpression->simplifyExpression();

	return simplifiedExpression;
	
	//Expression* simplifiedMergedExponent = performAssociativeOperationAndSimplify(firstExponent, secondExponent, "+");
	
	//Expression* simplifiedExponentiationExpression = getExponentiationExpression(firstExponentiationBinaryExpression->firstOperand, simplifiedMergedExponent);
	
	//delete simplifiedMergedExponent;
	//return simplifiedExponentiationExpression;
}
	
ExpressionPtr MultiplicationExpression::combineConstants(){
	double totalConstantValue = 1;
	std::vector<ExpressionPtr>::iterator op_iter = operands->begin();
	while(op_iter != operands->end()){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NUMERIC_CONSTANT){
			totalConstantValue *= std::dynamic_pointer_cast<NumericConstant>(operandExpression)->value;//(((NumericConstant*)operandExpression)->value);
			op_iter = operands->erase(op_iter);
		}
		else{
			++op_iter;
		}
	}

	if(totalConstantValue == 0){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		return zero;
		//return new NumericLiteral(0);
	}
	else if(operands->size() == 0){
		NumericConstantPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue);
		return totalConstant;
		//return new NumericLiteral(totalConstantValue);
	}
	else if(operands->size() == 1 && totalConstantValue == 1){
		return (*(operands->begin()))->deepcopy();
	}
	else{
		if(totalConstantValue != 1){
			NumericConstantPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue);
			operands->insert(operands->begin(), totalConstant);
			//operands->insert(operands->begin(), new NumericLiteral(totalConstantValue));
		}
		return deepcopy();
	}
}
	
void MultiplicationExpression::removeNegatedOperands(){
	double signValue = 1;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NEGATION_EXPRESSION){
			NegationExpressionPtr operandNegationExpression = std::dynamic_pointer_cast<NegationExpression>(operandExpression);
			//if(operandUnaryExpression->operation.compare("-") == 0){
			ExpressionPtr negatedOperand = operandNegationExpression->operand->deepcopy();
			*op_iter = negatedOperand;
			signValue *= -1;
			//}
		}
	}
	NumericConstantPtr sign = std::make_shared<NumericConstant>(signValue);
	operands->push_back(sign);
	//operands->push_back(new NumericLiteral(signValue));
}
	
MergeType MultiplicationExpression::getMergeType(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression){
	//MergeType constantMergeType = getConstantsMergeType(firstExpression, secondExpression);
	if(firstExpression->type == NUMERIC_CONSTANT && secondExpression->type == NUMERIC_CONSTANT){
		return TWO_NUMERIC_CONSTANTS;
	}
	else if(firstExpression->type == EXPONENTIATION_EXPRESSION && secondExpression->type == EXPONENTIATION_EXPRESSION){
		ExponentiationExpressionPtr firstExponentiationExpression = std::dynamic_pointer_cast<ExponentiationExpression>(firstExpression);
		ExponentiationExpressionPtr secondExponentiationExpression = std::dynamic_pointer_cast<ExponentiationExpression>(secondExpression);
		//if(firstBinaryNonCommutativeExpression->operation.compare("^") == 0 && secondBinaryNonCommutativeExpression->operation.compare("^") == 0){
		if(firstExponentiationExpression->firstOperand->equals(secondExponentiationExpression->firstOperand)){
			return MULTIPLICATION_TWO_EXPONENTIATION_EXPRESSIONS;
		}
		//}
	}
	else if(firstExpression->type == EXPONENTIATION_EXPRESSION){
		ExponentiationExpressionPtr firstExponentiationExpression = std::dynamic_pointer_cast<ExponentiationExpression>(firstExpression);
		//if(firstBinaryNonCommutativeExpression->operation.compare("^") == 0){
		if(firstExponentiationExpression->firstOperand->equals(secondExpression)){
			return MULTIPLICATION_EXPRESSION_WITH_EXPONENTIATION_EXPRESSION;
		}
		//}
	}
	else if(secondExpression->type == EXPONENTIATION_EXPRESSION){
		ExponentiationExpressionPtr secondExponentiationExpression = std::dynamic_pointer_cast<ExponentiationExpression>(secondExpression);
		//if(secondBinaryNonCommutativeExpression->operation.compare("^") == 0){
		if(secondExponentiationExpression->firstOperand->equals(firstExpression)){
			return MULTIPLICATION_EXPRESSION_WITH_EXPONENTIATION_EXPRESSION;
		}
		//}
	}
	else if(firstExpression->equals(secondExpression)){
		return TWO_EQUAL_EXPRESSIONS;
	}
	return NO_MERGE_POSSIBLE;
}

double MultiplicationExpression::getTotalConstantValue(){
	double constantValue = 1;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		ExpressionPtr operand = *op_iter;
		if(operand->type == NUMERIC_CONSTANT){
			constantValue *= std::dynamic_pointer_cast<NumericConstant>(operand)->value;
		}
	}
	return constantValue;
}
