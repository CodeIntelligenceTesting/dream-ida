////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "UnaryExpression.h"
#include "../../../Shared/ExpressionUtilities.h"

UnaryExpression::UnaryExpression(ExpressionType _expressionType, ExpressionPtr _operand) : Expression(_expressionType, NO_SUBSCRIPT)
{
	operand = _operand;
	//operation = _operation;
	isSimpleExpression = false;
}


UnaryExpression::~UnaryExpression(void)
{
	//delete operand; operand = NULL;
}


std::string UnaryExpression::getExpressionString(){
	return  "(" + getOperationString() + operand->getExpressionString() + ")";
}

char* UnaryExpression::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* operationCOLSTR = getColoredString(getOperationString(), CODE_COLOR);
	char* operandCOLSTR = operand->getColoredExpressionString();
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int operationCOLSTRSize = strlen(operationCOLSTR);
	int operandCOLSTRSize = strlen(operandCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);

	char* expCOLSTR = (char*)malloc(firstBracketCOLSTRSize + operandCOLSTRSize + operandCOLSTRSize + secondBracketCOLSTRSize + 1);
	memcpy(expCOLSTR, firstBracketCOLSTR, firstBracketCOLSTRSize);
	int currentPosition = firstBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, operationCOLSTR, operationCOLSTRSize);
	currentPosition += operationCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, operandCOLSTR, operandCOLSTRSize);
	currentPosition += operandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);
	currentPosition += secondBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);
	
	free(firstBracketCOLSTR);
	free(operationCOLSTR);
	free(operandCOLSTR);
	free(secondBracketCOLSTR);

	return expCOLSTR;
}

std::string UnaryExpression::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append("(");
	coloredString.append(getOperationString());
	coloredString.append(operand->getExpressionCOLSTR());
	coloredString.append(")");
	return coloredString;
}

//std::vector<Expression*>* UnaryExpression::getExpressionElements(bool includePointers){
//	return operand->getExpressionElements(includePointers);
//}

void UnaryExpression::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	createExpressionElements(operand, includePointers, elements_out);
}

void UnaryExpression::replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(!applyLocalSimplifications(operand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(operand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete operand;
			operand = replacingExpression->deepcopy();
		}
		else{
			operand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
}

bool UnaryExpression::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	return replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, operand);
}

bool UnaryExpression::equals(ExpressionPtr toThisExpression){
	if(type == toThisExpression->type){
		UnaryExpressionPtr toThisUnaryExpression = std::dynamic_pointer_cast<UnaryExpression>(toThisExpression);
		return operand->equals(toThisUnaryExpression->operand);
	}
	return false;
}
