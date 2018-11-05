////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "HighLevelCondition.h"
#include "../Constant/NumericConstant.h"
#include "../../../Shared/ExpressionUtilities.h"


HighLevelCondition::HighLevelCondition(std::string _comparisonOperand, ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, bool _isUnsigned) : Condition(HIGHLEVEL_CONDITION, NO_SUBSCRIPT)
{
	comparisonOperand = _comparisonOperand;
	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
	isSimpleExpression = false;
	isUnsigned = _isUnsigned;
}

HighLevelCondition::~HighLevelCondition(void)
{
	//delete firstOperand; firstOperand = NULL;
	//delete secondOperand;
}

std::string HighLevelCondition::getExpressionString(){
	return "(" + firstOperand->getExpressionString() + " " + comparisonOperand + " " + secondOperand->getExpressionString() + ")";
}

char* HighLevelCondition::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* firstOperandCOLSTR = firstOperand->getColoredExpressionString();
	char* secondOperandCOLSTR = secondOperand->getColoredExpressionString();
	char* comparisonOperandCOLSTR = getColoredString(" " + comparisonOperand + " ", CODE_COLOR);
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int firstOperandCOLSTRSize = strlen(firstOperandCOLSTR);
	int secondOperandCOLSTRSize = strlen(secondOperandCOLSTR);
	int comparisonOperandCOLSTRSize = strlen(comparisonOperandCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);

	char* expCOLSTR = (char*)malloc(firstBracketCOLSTRSize + firstOperandCOLSTRSize + secondOperandCOLSTRSize
		+ comparisonOperandCOLSTRSize + secondBracketCOLSTRSize + 1);
	memcpy(expCOLSTR, firstBracketCOLSTR, firstBracketCOLSTRSize);
	int currentPosition = firstBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, firstOperandCOLSTR, firstOperandCOLSTRSize);

	currentPosition += firstOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, comparisonOperandCOLSTR, comparisonOperandCOLSTRSize);

	currentPosition += comparisonOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, secondOperandCOLSTR, secondOperandCOLSTRSize);

	currentPosition += secondOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);

	currentPosition += secondBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);
	
	free(firstBracketCOLSTR);
	free(firstOperandCOLSTR);
	free(comparisonOperandCOLSTR);
	free(secondOperandCOLSTR);
	free(secondBracketCOLSTR);

	return expCOLSTR;
}

std::string HighLevelCondition::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append("(");
	coloredString.append(firstOperand->getExpressionCOLSTR());
	coloredString.append(" " + comparisonOperand + " ");
	coloredString.append(secondOperand->getExpressionCOLSTR());
	coloredString.append(")");
	return coloredString;
}

std::string HighLevelCondition::to_json(){
	std::string json_string = "{\"expression_type\":\"HighLevelCondition\", ";
	json_string.append("\"firstOperand\":");
	json_string.append(firstOperand->to_json() + ", ");
	json_string.append("\"comparisonOperand\":\"" + comparisonOperand + "\", ");
	json_string.append("\"secondOperand\":");
	json_string.append(secondOperand->to_json() + ", ");
	json_string.append("\"isUnsigned\":");
	json_string.append(isUnsigned ? "true" : "false");
	json_string.append("}");
	return json_string;
}

//std::vector<Expression*>* HighLevelCondition::getExpressionElements(bool includePointers){
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

void HighLevelCondition::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	createExpressionElements(firstOperand, includePointers, elements_out);
	createExpressionElements(secondOperand, includePointers, elements_out);
}

ExpressionPtr HighLevelCondition::deepcopy(){
	HighLevelConditionPtr copyPtr = std::make_shared<HighLevelCondition>(comparisonOperand, firstOperand->deepcopy() , secondOperand->deepcopy(), isUnsigned);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

void HighLevelCondition::replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(!applyLocalSimplifications(firstOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(firstOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete firstOperand;
			firstOperand = replacingExpression->deepcopy();
		}
		else if(true || firstOperand->type != POINTER){
			firstOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
	if(!applyLocalSimplifications(secondOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(secondOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete secondOperand;
			secondOperand = replacingExpression->deepcopy();
		}
		else if(true || secondOperand->type != POINTER){
			secondOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
}

bool HighLevelCondition::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool firstOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, firstOperand);
	bool secondOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, secondOperand);
	return firstOperandReplaced || secondOperandReplaced;
}

bool HighLevelCondition::isConditionTrue(){
	return areOperandsConstants() ? std::dynamic_pointer_cast<NumericConstant>(firstOperand)->value == std::dynamic_pointer_cast<NumericConstant>(secondOperand)->value : false;
}

bool HighLevelCondition::isConditionFalse(){
	return areOperandsConstants() ? std::dynamic_pointer_cast<NumericConstant>(firstOperand)->value != std::dynamic_pointer_cast<NumericConstant>(secondOperand)->value : false;
}

bool HighLevelCondition::areOperandsConstants(){
	return firstOperand->type == NUMERIC_CONSTANT && secondOperand->type == NUMERIC_CONSTANT;
}
