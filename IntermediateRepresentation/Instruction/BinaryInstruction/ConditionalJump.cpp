////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ConditionalJump.h"
#include "../../../Shared/ExpressionUtilities.h"

ConditionalJump::ConditionalJump(ExpressionPtr _condition, int _target) : BinaryInstruction(CONDITIONAL_JUMP)
{
	condition = _condition;
	target = _target;
}

ConditionalJump::~ConditionalJump(void)
{
	//msg("deleting.. %s\n", getInstructionString().c_str());
	//delete condition; condition = NULL;
}

std::string ConditionalJump::getInstructionString(){
	return "If " + condition->getExpressionString() + ", goto " + boost::lexical_cast<std::string>(target);
}

char* ConditionalJump::getColoredInstructionString(){
	char* ifCOLSTR = getColoredString("if ", CODE_COLOR);
	char* conditionCOLSTR = condition->getColoredExpressionString();
	char* gotoCOLSTR = getColoredString(", goto ", CODE_COLOR);
	char* targetCOLSTR = getColoredString(boost::lexical_cast<std::string>(target), NUMBER_COLOR);

	int ifCOLSTRSize = strlen(ifCOLSTR);
	int conditionCOLSTRSize = strlen(conditionCOLSTR);
	int gotoCOLSTRSize = strlen(gotoCOLSTR);
	int targetCOLSTRSize = strlen(targetCOLSTR);

	int instCOLSTRSize = ifCOLSTRSize + conditionCOLSTRSize + gotoCOLSTRSize + targetCOLSTRSize + 1;
	char* instCOLSTR = (char*)malloc(instCOLSTRSize);
	memcpy(instCOLSTR, ifCOLSTR, ifCOLSTRSize);

	int currentPosition = ifCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, conditionCOLSTR, conditionCOLSTRSize);
	
	currentPosition += conditionCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, gotoCOLSTR, gotoCOLSTRSize);

	currentPosition += gotoCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, targetCOLSTR, targetCOLSTRSize);

	currentPosition += targetCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, "\0", 1);

	free(ifCOLSTR);
	free(conditionCOLSTR);
	free(gotoCOLSTR);
	free(targetCOLSTR);

	return instCOLSTR;
}

std::string ConditionalJump::getInstructionCOLSTR(){
	std::string coloredString;
	coloredString.append("if ");
	coloredString.append(condition->getExpressionCOLSTR());
	coloredString.append(", goto ");
	coloredString.append(getCOLSTR(boost::lexical_cast<std::string>(target), NUMBER_COLOR));
	return coloredString;
}

//std::vector<Expression*>* ConditionalJump::getUsedElements(){
//	return condition->getExpressionElements(true);
//}
//
//std::vector<Expression*>* ConditionalJump::getDefinedElements(){
//	return NULL;
//}

void ConditionalJump::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(condition, true, usedElements_out);
}

void ConditionalJump::replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	condition->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
	replaceBySimplifiedExpression(condition);
}

bool ConditionalJump::replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	return condition->replaceChildExpressionNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
}

InstructionPtr ConditionalJump::deepcopy(){
	return std::make_shared<ConditionalJump>(condition->deepcopy(), target);
}
