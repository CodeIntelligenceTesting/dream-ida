////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Assignment.h"
#include "../../Expression/PointerExp.h"
#include "../../../Shared/ExpressionUtilities.h"

Assignment::Assignment(ExpressionPtr _lhsOperand, ExpressionPtr _rhsOperand) : BinaryInstruction(ASSIGNMENT)
{
	lhsOperand = _lhsOperand;
	rhsOperand = _rhsOperand;
}

Assignment::~Assignment(void)
{
	//msg("deleting.. %s\n", getInstructionString().c_str());
	//delete lhsOperand; lhsOperand = NULL;
	//delete rhsOperand; rhsOperand = NULL;
}

std::string Assignment::getInstructionString(){
	return lhsOperand->expressionType->getTypeCOLSTR() + lhsOperand->getExpressionString() + " = " + rhsOperand->getExpressionString();
}

char* Assignment::getColoredInstructionString(){
	char* lhsOperandCOLSTR = lhsOperand->getColoredExpressionString();
	char* equalCOLSTR = getColoredString(" = ", CODE_COLOR);
	char* rhsOperandCOLSTR = rhsOperand->getColoredExpressionString();
	int lhsOperandCOLSTRSize = strlen(lhsOperandCOLSTR);
	int equalCOLSTRSize = strlen(equalCOLSTR);
	int rhsOperandCOLSTRSize = strlen(rhsOperandCOLSTR);

	int instCOLSTRSize = lhsOperandCOLSTRSize + equalCOLSTRSize + rhsOperandCOLSTRSize + 1;
	char* instCOLSTR = (char*)malloc(instCOLSTRSize);
	memcpy(instCOLSTR, lhsOperandCOLSTR, lhsOperandCOLSTRSize);
	
	int currentPosition = lhsOperandCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, equalCOLSTR, equalCOLSTRSize);

	currentPosition += equalCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, rhsOperandCOLSTR, rhsOperandCOLSTRSize);

	currentPosition += rhsOperandCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, "\0", 1);

	free(lhsOperandCOLSTR);
	free(equalCOLSTR);
	free(rhsOperandCOLSTR);

	return instCOLSTR;
}

std::string Assignment::getInstructionCOLSTR(){
	std::string coloredString;// = lhsOperand->expressionType->getTypeCOLSTR();
	coloredString.append(lhsOperand->getExpressionCOLSTR());
	coloredString.append(" = ");
	coloredString.append(rhsOperand->getExpressionCOLSTR());
	return coloredString;
}

std::string Assignment::to_json(){
	return "{\"instruction_type\":\"Assignment\", \"lhsOperand\":" + lhsOperand->to_json() + ", \"rhsOperand\":" + rhsOperand->to_json() + "}";
}

//std::vector<Expression*>* Assignment::getUsedElements(){
//	std::vector<Expression*>* usedElements = rhsOperand->getExpressionElements(true);
//	if(lhsOperand->type == POINTER){
//		std::vector<Expression*>* addressElements = lhsOperand->getExpressionElements(false);
//		usedElements->insert(usedElements->end(), addressElements->begin(), addressElements->end());
//		delete addressElements;
//	}
//	return usedElements;
//}
//
//std::vector<Expression*>* Assignment::getDefinedElements(){
//	if(lhsOperand->type == POINTER){
//		std::vector<Expression*>* definedElements = new std::vector<Expression*>();
//		definedElements->push_back(lhsOperand);
//		return definedElements;
//	}
//	else{
//		return lhsOperand->getExpressionElements(true);
//	}
//}

void Assignment::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(rhsOperand, true, usedElements_out);
	if(lhsOperand->type == POINTER)
		lhsOperand->getExpressionElements(false, usedElements_out);
}

void Assignment::getDefinedElements(std::vector<ExpressionPtr>& definedElements_out){
	if(lhsOperand->type == POINTER)
		definedElements_out.push_back(lhsOperand);
	else
		createExpressionElements(lhsOperand, true, definedElements_out);
}

void Assignment::replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(!applyLocalSimplifications(rhsOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(rhsOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete rhsOperand;
			rhsOperand = replacingExpression->deepcopy();
		}
		else{
			rhsOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
		replaceBySimplifiedExpression(rhsOperand);
	}
	lhsOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
	replaceBySimplifiedExpression(lhsOperand);
}

bool Assignment::replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	//msg("lhsOperand(%s_%d)\n", lhsOperand->getName().c_str(), lhsOperand->subscript);
	return replaceOperand(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, lhsOperand);
}

bool Assignment::replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool result = replaceOperand(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, rhsOperand);
	if(lhsOperand->type == POINTER){
		result |= replaceOperand(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, std::dynamic_pointer_cast<PointerExp>(lhsOperand)->addressExpression);
	}
	return result;
}

bool Assignment::replaceOperand(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression, ExpressionPtr& operand){
	//Expression* operand = *operandPtr;
	if(operand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
		//msg("*** %s -->> %s\n", operand->getExpressionString().c_str(), replacingExpression->getExpressionString().c_str());
		//delete operand;
		operand = replacingExpression->deepcopy();
		return true;
	}
	return operand->replaceChildExpressionNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
}

InstructionPtr Assignment::deepcopy(){
	return std::make_shared<Assignment>(lhsOperand->deepcopy(), rhsOperand->deepcopy());
}
