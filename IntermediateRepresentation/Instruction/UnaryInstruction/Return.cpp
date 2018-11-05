////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Return.h"
#include "../../../Shared/ExpressionUtilities.h"


Return::Return(ExpressionPtr _operand) : UnaryInstruction(RETURN)
{
	operand = _operand;
}


Return::~Return(void)
{
	//delete operand; operand = NULL;
}


std::string Return::getInstructionString(){
	return "return " + operand->getExpressionString();
}

std::string Return::to_json(){
	return "{\"instruction_type\":\"Return\", \"operand\":" + operand->to_json() + "}";
}

char* Return::getColoredInstructionString(){
	char* returnCOLSTR = getColoredString("return ", CODE_COLOR);
	char* operandCOLSTR = operand->getColoredExpressionString();

	int returnCOLSTRSize = strlen(returnCOLSTR);
	int operandCOLSTRSize = strlen(operandCOLSTR);

	char* instCOLSTR = (char*)malloc(returnCOLSTRSize + operandCOLSTRSize + 1);
	memcpy(instCOLSTR, returnCOLSTR, returnCOLSTRSize);

	int currentPosition = returnCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, operandCOLSTR, operandCOLSTRSize);

	currentPosition += operandCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, "\0", 1);

	free(returnCOLSTR);
	free(operandCOLSTR);

	return instCOLSTR;
}

std::string Return::getInstructionCOLSTR(){
	std::string coloredString;
	coloredString.append("return ");
	coloredString.append(operand->getExpressionCOLSTR());
	return coloredString;
}

//std::vector<Expression*>* Return::getUsedElements(){
//	return operand->getExpressionElements(true);
//}


//std::vector<Expression*>* Return::getDefinedElements(){
//	return NULL;
//}

void Return::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(operand, true, usedElements_out);
}


void Return::replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(!applyLocalSimplifications(operand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(operand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete operand;
			operand = replacingExpression;
		}
		else{
			operand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
		replaceBySimplifiedExpression(operand);
	}
}

bool Return::replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(operand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
		//delete operand;
		operand = replacingExpression->deepcopy();
		return true;
	}
	return operand->replaceChildExpressionNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
}

InstructionPtr Return::deepcopy(){
	return std::make_shared<Return>(operand->deepcopy());
}
