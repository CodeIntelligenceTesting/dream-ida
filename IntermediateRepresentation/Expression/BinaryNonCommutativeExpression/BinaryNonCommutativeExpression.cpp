////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "BinaryNonCommutativeExpression.h"
//#include "../NumericLiteral.h"
#include "../../../Shared/ExpressionUtilities.h"


BinaryNonCommutativeExpression::BinaryNonCommutativeExpression(ExpressionType _expressionType, ExpressionPtr _firstOperand, ExpressionPtr _secondOperand) : Expression(_expressionType, NO_SUBSCRIPT)
{
	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
	isSimpleExpression = false;
}

BinaryNonCommutativeExpression::~BinaryNonCommutativeExpression(void)
{
	/*delete firstOperand; firstOperand = NULL;
	delete secondOperand; secondOperand = NULL;*/
}

std::string BinaryNonCommutativeExpression::getExpressionString(){
	return "(" + firstOperand->getExpressionString() + " " + getOperationString() + " " + secondOperand->getExpressionString() + ")";
}

char* BinaryNonCommutativeExpression::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* firstOperandCOLSTR = firstOperand->getColoredExpressionString();
	char* secondOperandCOLSTR = secondOperand->getColoredExpressionString();
	char* operationCOLSTR = getColoredString(" " + getOperationString() + " ", CODE_COLOR);
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int firstOperandCOLSTRSize = strlen(firstOperandCOLSTR);
	int secondOperandCOLSTRSize = strlen(secondOperandCOLSTR);
	int operationCOLSTRSize = strlen(operationCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);

	char* expCOLSTR = (char*)malloc(firstBracketCOLSTRSize + firstOperandCOLSTRSize + secondOperandCOLSTRSize
		+ operationCOLSTRSize + secondBracketCOLSTRSize + 1);
	memcpy(expCOLSTR, firstBracketCOLSTR, firstBracketCOLSTRSize);
	int currentPosistion = firstBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosistion, firstOperandCOLSTR, firstOperandCOLSTRSize);
	
	currentPosistion += firstOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosistion, operationCOLSTR, operationCOLSTRSize);
	
	currentPosistion += operationCOLSTRSize;
	memcpy(expCOLSTR + currentPosistion, secondOperandCOLSTR, secondOperandCOLSTRSize);
	
	currentPosistion += secondOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosistion, secondBracketCOLSTR, secondBracketCOLSTRSize);
	
	currentPosistion += secondBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosistion, "\0", 1);
	
	free(firstBracketCOLSTR);
	free(firstOperandCOLSTR);
	free(operationCOLSTR);
	free(secondOperandCOLSTR);
	free(secondBracketCOLSTR);

	return expCOLSTR;
}

std::string BinaryNonCommutativeExpression::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append("(");
	coloredString.append(firstOperand->getExpressionCOLSTR());
	coloredString.append(" " + getOperationString() + " ");
	coloredString.append(secondOperand->getExpressionCOLSTR());
	coloredString.append(")");
	return coloredString;
}

//std::vector<Expression*>* BinaryNonCommutativeExpression::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	
//	std::vector<Expression*>* firstOperandElements = firstOperand->getExpressionElements(includePointers);
//	expElements->insert(expElements->end(), firstOperandElements->begin(), firstOperandElements->end());
//	delete firstOperandElements;
//
//	std::vector<Expression*>* secondOperandElements = secondOperand->getExpressionElements(includePointers);
//	expElements->insert(expElements->end(), secondOperandElements->begin(), secondOperandElements->end());
//	delete secondOperandElements;
//
//	return expElements;
//}

void BinaryNonCommutativeExpression::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	createExpressionElements(firstOperand, includePointers, elements_out);
	createExpressionElements(secondOperand, includePointers, elements_out);
}

bool BinaryNonCommutativeExpression::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool firstOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, firstOperand);
	bool secondOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, secondOperand);
	return firstOperandReplaced || secondOperandReplaced;
}

void BinaryNonCommutativeExpression::replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(!applyLocalSimplifications(firstOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(firstOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete firstOperand;
			firstOperand = replacingExpression->deepcopy();
			//simplifyExpression();
		}
		else{
			firstOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}

	if(!applyLocalSimplifications(secondOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(secondOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete secondOperand;
			secondOperand = replacingExpression->deepcopy();
			//why haven't I written simplifyExpression();
		}
		else{
			secondOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
}

void BinaryNonCommutativeExpression::simplifyOperands(){
	replaceBySimplifiedExpression(firstOperand);
	replaceBySimplifiedExpression(secondOperand);
}

bool BinaryNonCommutativeExpression::equals(ExpressionPtr toThisExpression){
	if(type == toThisExpression->type){
		BinaryNonCommutativeExpressionPtr toThisBinaryNonCommutativeExpression = std::dynamic_pointer_cast<BinaryNonCommutativeExpression>(toThisExpression);
		return firstOperand->equals(toThisBinaryNonCommutativeExpression->firstOperand) && secondOperand->equals(toThisBinaryNonCommutativeExpression->secondOperand);
	}
	return false;
}
