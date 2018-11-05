////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "AdditionExpression.h"
#include "MultiplicationExpression.h"
#include "../Constant/NumericConstant.h"
#include "../UnaryExpression/NegationExpression.h"
#include "../TernaryExpression.h"

#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../../TypeAnalysis/Types/SimpleType/PointerType.h"
#include "../../../TypeAnalysis/Types/TopType.h"

AdditionExpression::AdditionExpression(ExpressionVectorPtr _operands) : AssociativeCommutativeExpression(_operands, ADDITION_EXPRESSION)
{
}

AdditionExpression::~AdditionExpression(void)
{
}

ExpressionPtr AdditionExpression::deepcopy(){
	ExpressionVectorPtr operandsCopy = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		operandsCopy->push_back((*op_iter)->deepcopy());
	}
	AdditionExpressionPtr copyAdd = std::make_shared<AdditionExpression>(operandsCopy);
	copyAdd->size_in_bytes = size_in_bytes;
	copyAdd->expressionType = expressionType->deepcopy();
	return copyAdd;
}

ExpressionPtr AdditionExpression::simplifyExpression(){
	simplifyOperands();
	for(std::vector<ExpressionPtr>::iterator currentOp_iter = operands->begin() ; currentOp_iter != operands->end() ; ++currentOp_iter){
		ExpressionPtr currentOperand = *currentOp_iter;
		std::vector<ExpressionPtr>::iterator nextOp_iter = currentOp_iter + 1;
		while(nextOp_iter != operands->end()){
			ExpressionPtr nextOperand = *nextOp_iter;
			MergeInfo additionMergeInfo = getMergeInfo(currentOperand, nextOperand);
			switch(additionMergeInfo.first){
			case TWO_NUMERIC_CONSTANTS:
				{
					NumericConstantPtr currentOperandPtr = std::dynamic_pointer_cast<NumericConstant>(currentOperand);
					NumericConstantPtr nextOperandPtr = std::dynamic_pointer_cast<NumericConstant>(nextOperand);
					currentOperandPtr->value += nextOperandPtr->value;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case TERNARY_EXPRESSION_WITH_CONSTANT:
				{
					ExpressionPtr variableExp = (currentOperand->type != NUMERIC_CONSTANT) ? currentOperand : nextOperand;
					ExpressionPtr constantExp = (currentOperand->type == NUMERIC_CONSTANT) ? currentOperand : nextOperand;
					ExpressionPtr nonNegatedVariableExp = (variableExp->type == NEGATION_EXPRESSION) ? std::dynamic_pointer_cast<NegationExpression>(variableExp)->operand : variableExp;
					if(nonNegatedVariableExp->type == TERNARY_EXPRESSION && constantExp->type == NUMERIC_CONSTANT){
						TernaryExpressionPtr ternaryExp = std::dynamic_pointer_cast<TernaryExpression>(nonNegatedVariableExp);
						NumericConstantPtr constant = std::dynamic_pointer_cast<NumericConstant>(constantExp);
						if(ternaryExp->secondOperand->type == NUMERIC_CONSTANT && ternaryExp->thirdOperand->type == NUMERIC_CONSTANT){
							std::dynamic_pointer_cast<NumericConstant>(ternaryExp->secondOperand)->value += constant->value;
							std::dynamic_pointer_cast<NumericConstant>(ternaryExp->thirdOperand)->value += constant->value;
							constant->value = 0;
						}
					}
					++nextOp_iter;					
				}
				break;
			//case TWO_SYMBOLIC_CONSTANTS:
			case TWO_EQUAL_EXPRESSIONS:
				{
					ExpressionPtr mergedExpression;
					if(currentOperand->type == NEGATION_EXPRESSION){
						ExpressionPtr negatedOperand = std::dynamic_pointer_cast<NegationExpression>(currentOperand)->operand;
						mergedExpression = simplifyAdditionOfEqualExpressions(negatedOperand, additionMergeInfo.second);
					}
					else{
						mergedExpression = simplifyAdditionOfEqualExpressions(currentOperand, additionMergeInfo.second);
					}
					*currentOp_iter = mergedExpression;
					currentOperand = *currentOp_iter;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case ADDITION_EXPRESSION_WITH_OPERAND_IN_MULTIPLICATION_EXPRESSION:
				{
					ExpressionPtr mergedExpression;
					if(currentOperand->type == MULTIPLICATION_EXPRESSION){
						MultiplicationExpressionPtr currentOperandMult = std::dynamic_pointer_cast<MultiplicationExpression>(currentOperand);
						mergedExpression = simplifyAdditionOfOperandAndMultiplicationExpression(currentOperandMult,
																								 nextOperand,
																								 additionMergeInfo.second);
					}
					else{
						SignCombination reversedSignCombination = additionMergeInfo.second;
						if(reversedSignCombination == FIRST_NEGATIVE_SECOND_POSITIVE){
							reversedSignCombination = FIRST_POSITIVE_SECOND_NEGATIVE;
						}
						else if(reversedSignCombination == FIRST_POSITIVE_SECOND_NEGATIVE){
							reversedSignCombination = FIRST_NEGATIVE_SECOND_POSITIVE;
						}
						MultiplicationExpressionPtr nextOperandMult = std::dynamic_pointer_cast<MultiplicationExpression>(nextOperand);
						mergedExpression = simplifyAdditionOfOperandAndMultiplicationExpression(nextOperandMult,
																								 currentOperand,
																								 reversedSignCombination);
					}
					*currentOp_iter = mergedExpression;
					currentOperand = *currentOp_iter;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case ADDITION_TWO_MULTIPLICATION_EXPRESSIONS:
				{
					ExpressionPtr nonNegatedCurrentOp = (currentOperand->type == NEGATION_EXPRESSION) ? std::dynamic_pointer_cast<NegationExpression>(currentOperand)->operand : currentOperand;
					MultiplicationExpressionPtr currentOperandMult = std::dynamic_pointer_cast<MultiplicationExpression>(nonNegatedCurrentOp);

					ExpressionPtr nonNegatedNextOp = (nextOperand->type == NEGATION_EXPRESSION) ? std::dynamic_pointer_cast<NegationExpression>(nextOperand)->operand : nextOperand;
					MultiplicationExpressionPtr nextOperandMult = std::dynamic_pointer_cast<MultiplicationExpression>(nonNegatedNextOp);
					
					ExpressionPtr mergedExpression = simplifyAdditionOfTwoMultiplicationExpressions(currentOperandMult,
																								    nextOperandMult,
																								    additionMergeInfo.second);
					*currentOp_iter = mergedExpression;
					currentOperand = *currentOp_iter;
					nextOp_iter = operands->erase(nextOp_iter);
				}
				break;
			case NO_SIMPLIFICATION:
				++nextOp_iter;
			}
		}
	}
	ExpressionPtr simplifiedExpression = combineConstants();
	return simplifiedExpression;
}

std::string AdditionExpression::getOperationString(){
	return "+";
}

TypePtr AdditionExpression::getExpresstionType(TypeMapPtr typeMap){
	bool isOddNumberOfPointers = false;
	//bool areAllOperandsIntegers = true;
	
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		TypePtr opType = (*op_iter)->getExpresstionType(typeMap);
		if(opType->type == INTEGRAL_TYPE){
			IntegralTypePtr opIntegralType = std::dynamic_pointer_cast<IntegralType>(opType);
			if(opIntegralType->integralType == DOUBLE_TYPE){
				return std::make_shared<IntegralType>(DOUBLE_TYPE);
			}
		}
		else if(opType->type == POINTER_TYPE){
			//areAllOperandsIntegers = false;
			isOddNumberOfPointers = !isOddNumberOfPointers;
		}
		else{
			return std::make_shared<TopType>();
		}
	}

	if(/*areAllOperandsIntegers || */isOddNumberOfPointers)
		return std::make_shared<PointerType>();
	else
		return std::make_shared<IntegralType>(INT_TYPE);
}

void AdditionExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
	if(type->isFloatType()){
		for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
			(*op_iter)->updateExpressionType(type, typeMap);
		}
	}
	else{
		int numberOfUnresolvedTypes = 0;
		int numberOfPointers = 0;
		ExpressionPtr operandWithUnresolvedType;
		for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
			operandWithUnresolvedType = *op_iter;
			TypePtr operandType = operandWithUnresolvedType->getExpresstionType(typeMap);
			if(operandType->type == TOP_TYPE){
				numberOfUnresolvedTypes++;
				if(numberOfUnresolvedTypes > 1)
					return;
			}
			else if(operandType->isPointerType()){
				numberOfPointers++;
			}
		}

		if(numberOfUnresolvedTypes == 1){
			if(type->isIntegerType()){
				if(numberOfPointers % 2 == 1)
					operandWithUnresolvedType->updateExpressionType(std::make_shared<PointerType>(), typeMap);
				else
					operandWithUnresolvedType->updateExpressionType(std::make_shared<IntegralType>(INT_TYPE), typeMap);
			}
			else if(type->isPointerType()){
				if(numberOfPointers % 2 == 1)
					operandWithUnresolvedType->updateExpressionType(std::make_shared<IntegralType>(INT_TYPE), typeMap);
				else
					operandWithUnresolvedType->updateExpressionType(std::make_shared<PointerType>(), typeMap);
			}
		}
	}
}

ExpressionPtr AdditionExpression::simplifyAdditionOfEqualExpressions(const ExpressionPtr& expressionToBeMerged, SignCombination signCombination){
	if(signCombination == FIRST_NEGATIVE_SECOND_POSITIVE || signCombination == FIRST_POSITIVE_SECOND_NEGATIVE){
		NumericConstantPtr result = std::make_shared<NumericConstant>(0);
		return result;
	}
	else{
		ExpressionVectorPtr multOperands = std::make_shared<std::vector<ExpressionPtr>>();
		NumericConstantPtr constantOperand = std::make_shared<NumericConstant>(signCombination == BOTH_POSITIVE ? 2 : -2);
		multOperands->push_back(constantOperand);
		//operands->push_back((signCombination == BOTH_POSITIVE ? new NumericLiteral(2) : new NumericLiteral(-2)));
		multOperands->push_back(expressionToBeMerged->deepcopy());
		MultiplicationExpressionPtr result = std::make_shared<MultiplicationExpression>(multOperands);
		return result;
	}
}

ExpressionPtr AdditionExpression::simplifyAdditionOfOperandAndMultiplicationExpression(const MultiplicationExpressionPtr& multiplicationExpression, const ExpressionPtr& operand, SignCombination signCombination){
	for(std::vector<ExpressionPtr>::iterator op_iter = multiplicationExpression->operands->begin() ; op_iter != multiplicationExpression->operands->end() ; ++op_iter){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NUMERIC_CONSTANT){
			NumericConstantPtr operandNumericConstant = std::dynamic_pointer_cast<NumericConstant>(operandExpression);
			operandNumericConstant->value += (signCombination == BOTH_POSITIVE || signCombination == FIRST_NEGATIVE_SECOND_POSITIVE) ? 1 : -1;
			/*if(signCombination == BOTH_POSITIVE || signCombination == FIRST_NEGATIVE_SECOND_POSITIVE){
				operandNumericLiteral->numericLiteralValue.value++;
			}
			else{
				operandNumericLiteral->numericLiteralValue.value--;
			}*/
			if(operandNumericConstant->value == 0){
				NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
				return zero;
			}
			else if(operandNumericConstant->value == 1){
				multiplicationExpression->operands->erase(op_iter);
				break;
			}
		}
	}
	if(multiplicationExpression->operands->size() == 1){
		return (*(multiplicationExpression->operands->begin()))->deepcopy();
	}
	else{
		return multiplicationExpression->deepcopy();
	}
}

ExpressionPtr AdditionExpression::simplifyAdditionOfTwoMultiplicationExpressions(const MultiplicationExpressionPtr& firstMultExpression, const MultiplicationExpressionPtr& secondMultExpression, SignCombination signCombination){
	double firstConstantValue = firstMultExpression->getTotalConstantValue();
	double secondConstantValue = secondMultExpression->getTotalConstantValue();
	double totalConstantValue;
	if(signCombination == BOTH_POSITIVE || signCombination == FIRST_NEGATIVE_SECOND_POSITIVE){
		totalConstantValue = firstConstantValue + secondConstantValue;
	}
	else{
		totalConstantValue = firstConstantValue - secondConstantValue;
	}

	if(totalConstantValue == 0){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		return zero;
	}
	MultiplicationExpressionPtr mergedExpression = std::dynamic_pointer_cast<MultiplicationExpression>(firstMultExpression->deepcopy());
	mergedExpression->removeConstantOperands();
	if(totalConstantValue == 1){
		if(mergedExpression->operands->size() == 1){
			ExpressionPtr finalExpression = (*(mergedExpression->operands->begin()))->deepcopy();
			return finalExpression;
		}
		else{
			return mergedExpression;
		}
	}
	else if(totalConstantValue == -1){
		if(mergedExpression->operands->size() == 1){
			ExpressionPtr finalExpression = std::make_shared<NegationExpression>((*(mergedExpression->operands->begin()))->deepcopy());
			//Expression* finalExpression = new UnaryExpression("-", (*(mergedExpression->operands->begin()))->deepcopy());
			return finalExpression;
		}
		else{
			NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
			mergedExpression->operands->insert(mergedExpression->operands->begin(), minusOne);
			//mergedExpression->operands->insert(mergedExpression->operands->begin(), new NumericLiteral(-1));
			return mergedExpression;
		}
	}
	else{
		NumericConstantPtr totalConstant = std::make_shared<NumericConstant>(totalConstantValue);
		mergedExpression->operands->insert(mergedExpression->operands->begin(), totalConstant);
		//mergedExpression->operands->insert(mergedExpression->operands->begin(), new NumericLiteral(totalConstantValue));
		return mergedExpression;
	}
}

ExpressionPtr AdditionExpression::combineConstants(){
	double totalConstantValue = 0;
	std::vector<ExpressionPtr>::iterator op_iter = operands->begin();
	while(op_iter != operands->end()){
		ExpressionPtr operandExpression = *op_iter;
		if(operandExpression->type == NUMERIC_CONSTANT){
			totalConstantValue += std::dynamic_pointer_cast<NumericConstant>(operandExpression)->value;//(((NumericConstant*)operandExpression)->value);
			op_iter = operands->erase(op_iter);
		}
		else{
			++op_iter;
		}
		/*if(operandExpression->type == NUMERIC_LITERAL){
			NumericLiteral* operandNumericLiteral = (NumericLiteral*)operandExpression;
			if(!operandNumericLiteral->isSymbolicValue){
				totalConstantValue += operandNumericLiteral->numericLiteralValue.value;
				op_iter = operands->erase(op_iter);
			}
			else{
				++op_iter;
			}
		}
		else{
			++op_iter;
		}*/
	}

	if(operands->size() == 0){
		NumericConstantPtr constantResult = std::make_shared<NumericConstant>(totalConstantValue);
		return constantResult;
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
		NumericConstantPtr constantOperand = std::make_shared<NumericConstant>(totalConstantValue);
		operands->push_back(constantOperand);
		return deepcopy();
	}
}

MergeInfo AdditionExpression::getMergeInfo(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression){
	SignCombination signCombination = BOTH_POSITIVE;
	ExpressionPtr firstComparedExpression = firstExpression;
	ExpressionPtr secondComparedExpression = secondExpression;
	bool isFirstExpressionNegated = firstExpression->type == NEGATION_EXPRESSION;
	bool isSecondExpressionNegated = secondExpression->type == NEGATION_EXPRESSION;

	
	if(isFirstExpressionNegated && isSecondExpressionNegated){
		signCombination = BOTH_NEGATIVE;
		firstComparedExpression = std::dynamic_pointer_cast<UnaryExpression>(firstExpression)->operand;
		secondComparedExpression = std::dynamic_pointer_cast<UnaryExpression>(secondExpression)->operand;
	}
	else if(isFirstExpressionNegated){
		signCombination = FIRST_NEGATIVE_SECOND_POSITIVE;
		firstComparedExpression = std::dynamic_pointer_cast<UnaryExpression>(firstExpression)->operand;
	}
	else if(isSecondExpressionNegated){
		signCombination = FIRST_POSITIVE_SECOND_NEGATIVE;
		secondComparedExpression = std::dynamic_pointer_cast<UnaryExpression>(secondExpression)->operand;
	}

	//MergeType constantMergeType = getConstantsMergeType(firstExpression, secondExpression);
	if(firstComparedExpression->type == NUMERIC_CONSTANT && secondComparedExpression->type == NUMERIC_CONSTANT){
		return std::make_pair(TWO_NUMERIC_CONSTANTS, signCombination);
	}
	else if((firstComparedExpression->type == TERNARY_EXPRESSION && secondComparedExpression->type == NUMERIC_CONSTANT)
			  || (firstComparedExpression->type == NUMERIC_CONSTANT && secondComparedExpression->type == TERNARY_EXPRESSION)){
				  return std::make_pair(TERNARY_EXPRESSION_WITH_CONSTANT, signCombination);
	}
	/*MergeType constantMergeType = getConstantsMergeType(firstComparedExpression, secondComparedExpression);
	if(constantMergeType != NO_MERGE_POSSIBLE){
		return std::make_pair(constantMergeType, signCombination);
	}*/
	else if(firstComparedExpression->type == MULTIPLICATION_EXPRESSION && secondComparedExpression->type == MULTIPLICATION_EXPRESSION){
		AssociativeCommutativeExpressionPtr firstComp = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(firstComparedExpression);
		AssociativeCommutativeExpressionPtr secondComp = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(secondComparedExpression);
		if(equalsDiscardConstants(firstComp, secondComp)){
			return std::make_pair(ADDITION_TWO_MULTIPLICATION_EXPRESSIONS, signCombination);
		}
	}
	else if(firstComparedExpression->type == MULTIPLICATION_EXPRESSION){
		MultiplicationExpressionPtr firstComparedMultExp = std::dynamic_pointer_cast<MultiplicationExpression>(firstComparedExpression);
		if(canMergeWithMultiplicationExpression(firstComparedMultExp, secondComparedExpression)){
			return std::make_pair(ADDITION_EXPRESSION_WITH_OPERAND_IN_MULTIPLICATION_EXPRESSION, signCombination);
		}
	}
	else if(secondComparedExpression->type == MULTIPLICATION_EXPRESSION){
		MultiplicationExpressionPtr secondComparedMultExp = std::dynamic_pointer_cast<MultiplicationExpression>(secondComparedExpression);
		if(canMergeWithMultiplicationExpression(secondComparedMultExp, firstComparedExpression)){
			return std::make_pair(ADDITION_EXPRESSION_WITH_OPERAND_IN_MULTIPLICATION_EXPRESSION, signCombination);
		}
	}
	else if(firstComparedExpression->equals(secondComparedExpression)){
		return std::make_pair(TWO_EQUAL_EXPRESSIONS, signCombination);
	}
	return std::make_pair(NO_MERGE_POSSIBLE, signCombination);
}

bool AdditionExpression::canMergeWithMultiplicationExpression(const MultiplicationExpressionPtr& multExp, const ExpressionPtr& operandExpression){
	bool result = false;
	MultiplicationExpressionPtr multiplicationExpressionCopy = std::dynamic_pointer_cast<MultiplicationExpression>(multExp->deepcopy());
	multiplicationExpressionCopy->removeConstantOperands();
	if(multiplicationExpressionCopy->operands->size() == 1){
		result = operandExpression->equals(*(multiplicationExpressionCopy->operands->begin()));
	}
	return result;
}
