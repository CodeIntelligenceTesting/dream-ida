////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "MathFunction.h"
#include "../../Shared/ExpressionUtilities.h"

MathFunction::MathFunction(MathFunctionType _mathFunctionType, ExpressionPtr _argument) : Expression(MATH_FUNCTION, NO_SUBSCRIPT)
{
	mathFunctionType = _mathFunctionType;
	argument = _argument;
	isSimpleExpression = false;
}


MathFunction::~MathFunction(void)
{
	//delete argument; argument = NULL;
}


std::string MathFunction::getFunctionName(){
std::string functionName;
	switch(mathFunctionType){
	case ABS:
		functionName = "abs";
		break;
	case COS:
		functionName = "cos";
		break;
	case LN:
		functionName = "ln";
		break;
	case LOG_2:
		functionName = "log_2";
		break;
	case LOG_10:
		functionName = "log_10";
		break;
	case PREM:
		functionName = "prem";
		break;
	case RNDINT:
		functionName = "roundToInt";
		break;
	case SIN:
		functionName = "sin";
		break;
	case SQRT:
		functionName = "sqrt";
		break;
	case TAN:
		functionName = "tan";
		break;
	case TRUNCATE:
		functionName = "truncate";
		break;
	case TWO_X:
		functionName = "pow_2";
		break;
	}
	return functionName;
}


std::string MathFunction::getExpressionString(){
	return getFunctionName() + "(" + argument->getExpressionString() + ")";
}


char* MathFunction::getColoredExpressionString(){
	char* functionNameCOLSTR = getColoredString(getFunctionName(), MACRO_COLOR);
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	char* argumentCOLSTR = argument->getColoredExpressionString();
	
	int functionNameCOLSTRSize = strlen(functionNameCOLSTR);
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);
	int argumentCOLSTRSize = strlen(argumentCOLSTR);

	char* functionCOLSTR = (char*)malloc(functionNameCOLSTRSize + firstBracketCOLSTRSize + secondBracketCOLSTRSize + argumentCOLSTRSize + 1);
	memcpy(functionCOLSTR, functionNameCOLSTR, functionNameCOLSTRSize);
	int currentPosition = functionNameCOLSTRSize;

	memcpy(functionCOLSTR + currentPosition, firstBracketCOLSTR, firstBracketCOLSTRSize);
	currentPosition += firstBracketCOLSTRSize;

	memcpy(functionCOLSTR + currentPosition, argumentCOLSTR, argumentCOLSTRSize);
	currentPosition += argumentCOLSTRSize;

	memcpy(functionCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);
	currentPosition += secondBracketCOLSTRSize;

	memcpy(functionCOLSTR + currentPosition, "\0", 1);

	free(functionNameCOLSTR);
	free(firstBracketCOLSTR);
	free(secondBracketCOLSTR);
	free(argumentCOLSTR);

	return functionCOLSTR;
}

std::string MathFunction::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append(getCOLSTR(getFunctionName(), MACRO_COLOR));
	coloredString.append("(");
	coloredString.append(argument->getExpressionCOLSTR());
	coloredString.append(")");
	return coloredString;
}

//std::vector<Expression*>* MathFunction::getExpressionElements(bool includePointers){
//	return argument->getExpressionElements(includePointers);
//}

void MathFunction::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	createExpressionElements(argument, includePointers, elements_out);
}


ExpressionPtr MathFunction::deepcopy(){
	MathFunctionPtr copyPtr = std::make_shared<MathFunction>(mathFunctionType, argument->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

bool MathFunction::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	return replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, argument);
}
