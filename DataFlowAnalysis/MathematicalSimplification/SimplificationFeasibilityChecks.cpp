////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <string>

#include "SimplificationFeasibilityChecks.h"
#include "../../IntermediateRepresentation/Expression/Expression.h"
#include "../../IntermediateRepresentation/Expression/ExpressionTypes.h"
#include "../../IntermediateRepresentation/Expression/AssociativeCommutativeExpression/AssociativeCommutativeExpression.h"
#include "../../IntermediateRepresentation/Expression/BinaryNonCommutativeExpression/ExponentiationExpression.h"
#include "../../IntermediateRepresentation/Expression/Constant/NumericConstant.h"
#include "../../IntermediateRepresentation/Expression/UnaryExpression/NegationExpression.h"
#include "../../IntermediateRepresentation/Expression/UnaryExpression/LogicalNotExpression.h"


SimplificationType getSimplificationType(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(doesPropagationResultsInConstantNegation(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return CONSTANT_NEGATION;
	}
	/*else if(doesPropagationResultsInDoubleUnaryExpression(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return DOUBLE_UNARY_EXPRESSION;
	}*/
	else if(doesPropagationResultsInDoubleLogicalNOT(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return DOUBLE_LOGICAL_NOT;
	}
	else if(doesPropagationResultsInDoubleNegation(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return DOUBLE_NEGATION;
	}
	else if(doesPropagationResultsInNegationOfAdditionOrMultiplication(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(replacingExpression->type == ADDITION_EXPRESSION){
			return NEGATION_OF_ADDITION;
		}

		return NEGATION_OF_MULTIPLICATION;
	}
	else if(doesPropagationResultsInDoubleExponentiation(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return DOUBLE_EXPONENTIATION;
	}
	else if(doesPropagationResultsInExponentiationOfMultiplication(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return EXPONENTIATION_OF_MYLTIPLICATION;
	}
	/*else if(doesPropagationResultsInMultiplicationWithDivision(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		return MYLTIPLICATION_TO_DIVISION;
	}*/
	else{
		return NO_SIMPLIFICATION;//getPotentialDoubleDivisionSimplificationType(operandExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
	}
}


bool doesPropagationResultsInConstantNegation(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(replacingExpression->type == NUMERIC_CONSTANT && operandExpression->type == NEGATION_EXPRESSION){
		//NumericLiteral* replacingNumericLiteral = (NumericLiteral*)replacingExpression;
		//if(!replacingNumericLiteral->isSymbolicValue){
		
		ExpressionPtr negatedOperand = std::dynamic_pointer_cast<NegationExpression>(operandExpression)->operand;
		return negatedOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced);// nameToBeReplaced.compare(negatedOperand->getName()) == 0 && subscriptToBeReplaced == negatedOperand->subscript;
		//}
	}
	return false;
}


bool doesPropagationResultsInDoubleNegation(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(replacingExpression->type == NEGATION_EXPRESSION && operandExpression->type == NEGATION_EXPRESSION){
		ExpressionPtr negatedOperand = std::dynamic_pointer_cast<NegationExpression>(operandExpression)->operand;
		return negatedOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced);
	}
	return false;
}


bool doesPropagationResultsInDoubleLogicalNOT(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(replacingExpression->type == LOGICAL_NOT_EXPRESSION && operandExpression->type == LOGICAL_NOT_EXPRESSION){
		ExpressionPtr operand = std::dynamic_pointer_cast<LogicalNotExpression>(operandExpression)->operand;
		return operand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced);
	}
	return false;
}


bool doesPropagationResultsInNegationOfAdditionOrMultiplication(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if((replacingExpression->type == ADDITION_EXPRESSION || replacingExpression->type == MULTIPLICATION_EXPRESSION) && operandExpression->type == NEGATION_EXPRESSION){
		ExpressionPtr negatedOperand = std::dynamic_pointer_cast<NegationExpression>(operandExpression)->operand;
		return negatedOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced);//nameToBeReplaced.compare(operandNegationExpression->operand->getName()) == 0 && subscriptToBeReplaced == operandNegationExpression->operand->subscript;
	}
	return false;
}


bool doesPropagationResultsInDoubleExponentiation(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(operandExpression->type == EXPONENTIATION_EXPRESSION && replacingExpression->type == EXPONENTIATION_EXPRESSION){
		ExpressionPtr operandBase = std::dynamic_pointer_cast<ExponentiationExpression>(operandExpression)->firstOperand;
		return operandBase->isSameVariable(nameToBeReplaced, subscriptToBeReplaced);
	}
	return false;
}


bool doesPropagationResultsInExponentiationOfMultiplication(const ExpressionPtr& operandExpression, const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(operandExpression->type == EXPONENTIATION_EXPRESSION && replacingExpression->type == MULTIPLICATION_EXPRESSION){
		ExpressionPtr operandBase = std::dynamic_pointer_cast<ExponentiationExpression>(operandExpression)->firstOperand;
		return operandBase->isSameVariable(nameToBeReplaced, subscriptToBeReplaced);
	}
	return false;
}


//SimplificationType getPotentialDoubleDivisionSimplificationType(Expression* operandExpression, std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression){
//	if(replacingExpression->type == BINARY_NON_COMMUTATIVE_EXPRESSION && operandExpression->type == BINARY_NON_COMMUTATIVE_EXPRESSION){
//		BinaryNonCommutativeExpression* operandBinaryExpression = (BinaryNonCommutativeExpression*)operandExpression;
//		BinaryNonCommutativeExpression* replacingBinaryExpression = (BinaryNonCommutativeExpression*)replacingExpression;
//		if(operandBinaryExpression->operation.compare("/") == 0 && replacingBinaryExpression->operation.compare("/") == 0){
//			Expression* operandDividend = operandBinaryExpression->firstOperand;
//			if(operandDividend->isSimpleExpression){
//				if(nameToBeReplaced.compare(operandDividend->getName()) == 0 && subscriptToBeReplaced == operandDividend->subscript){
//					return DOUBLE_DIVISION_DIVIDEND_SIMPLE;
//				}
//			}
//			else if(operandDividend->type == ASSICIATIVE_COMMUTATIVE_EXPRESSION){
//				AssociativeCommutativeExpression* operandDividendAssCommExp = (AssociativeCommutativeExpression*)operandDividend;
//				if(operandDividendAssCommExp->operation.compare("*") == 0){
//					for(std::vector<Expression*>::iterator subOp_iter = operandDividendAssCommExp->operands->begin() ; subOp_iter != operandDividendAssCommExp->operands->end() ; ++subOp_iter){
//						Expression* subOperandExp = *subOp_iter;
//						if(nameToBeReplaced.compare(subOperandExp->getName()) == 0 && subscriptToBeReplaced == subOperandExp->subscript){
//							return DOUBLE_DIVISION_DIVIDEND_AGGREGATE;
//						}
//					}
//				}
//			}
//
//			Expression* operandDivisor = operandBinaryExpression->secondOperand;
//			if(operandDivisor->isSimpleExpression){
//				if(nameToBeReplaced.compare(operandDivisor->getName()) == 0 && subscriptToBeReplaced == operandDivisor->subscript){
//					return DOUBLE_DIVISION_DIVISOR_SIMPLE;
//				}
//			}
//			else if(operandDivisor->type == ASSICIATIVE_COMMUTATIVE_EXPRESSION){
//				AssociativeCommutativeExpression* operandDivisorAssCommExp = (AssociativeCommutativeExpression*)operandDivisor;
//				if(operandDivisorAssCommExp->operation.compare("*") == 0){
//					for(std::vector<Expression*>::iterator subOp_iter = operandDivisorAssCommExp->operands->begin() ; subOp_iter != operandDivisorAssCommExp->operands->end() ; ++subOp_iter){
//						Expression* subOperandExp = *subOp_iter;
//						if(nameToBeReplaced.compare(subOperandExp->getName()) == 0 && subscriptToBeReplaced == subOperandExp->subscript){
//							return DOUBLE_DIVISION_DIVISOR_AGGREGATE;
//						}
//					}
//				}
//			}
//		}
//	}
//	return NO_SIMPLIFICATION;
//}
