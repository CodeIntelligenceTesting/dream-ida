////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TernaryExpression.h"
#include "Condition/HighLevelCondition.h"
#include "../../Shared/ExpressionUtilities.h"


TernaryExpression::TernaryExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _thirdOperand) : Expression(TERNARY_EXPRESSION, NO_SUBSCRIPT)
{
	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
	thirdOperand = _thirdOperand;
	isSimpleExpression = false;
}


TernaryExpression::~TernaryExpression(void)
{
	/*delete firstOperand; firstOperand = NULL;
	delete secondOperand; secondOperand = NULL;
	delete thirdOperand; thirdOperand = NULL;*/
}


std::string TernaryExpression::getExpressionString(){
	return "(" + firstOperand->getExpressionString() + " ? " + secondOperand->getExpressionString() + " : " + thirdOperand->getExpressionString() + ")";
}


char* TernaryExpression::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* firstOperandCOLSTR = firstOperand->getColoredExpressionString();
	char* questionMarkCOLSTR = getColoredString(" ? ", CODE_COLOR);
	char* secondOperandCOLSTR = secondOperand->getColoredExpressionString();
	char* colonCOLSTR = getColoredString(" : ", CODE_COLOR);
	char* thirdOperandCOLSTR = thirdOperand->getColoredExpressionString();
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int firstOperandCOLSTRSize = strlen(firstOperandCOLSTR);
	int questionMarkCOLSTRSize = strlen(questionMarkCOLSTR);
	int secondOperandCOLSTRSize = strlen(secondOperandCOLSTR);
	int colonCOLSTRSize = strlen(colonCOLSTR);
	int thirdOperandCOLSTRSize = strlen(thirdOperandCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);

	char* expCOLSTR = (char*)malloc(firstBracketCOLSTRSize + firstOperandCOLSTRSize + questionMarkCOLSTRSize + secondOperandCOLSTRSize
		+ colonCOLSTRSize + thirdOperandCOLSTRSize + secondBracketCOLSTRSize + 1);
	memcpy(expCOLSTR, firstBracketCOLSTR, firstBracketCOLSTRSize);
	int currentPosition = firstBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, firstOperandCOLSTR, firstOperandCOLSTRSize);

	currentPosition += firstOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, questionMarkCOLSTR, questionMarkCOLSTRSize);

	currentPosition += questionMarkCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, secondOperandCOLSTR, secondOperandCOLSTRSize);

	currentPosition += secondOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, colonCOLSTR, colonCOLSTRSize);

	currentPosition += colonCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, thirdOperandCOLSTR, thirdOperandCOLSTRSize);

	currentPosition += thirdOperandCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);

	currentPosition += secondBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);
	
	free(firstBracketCOLSTR);
	free(firstOperandCOLSTR);
	free(questionMarkCOLSTR);
	free(secondOperandCOLSTR);
	free(colonCOLSTR);
	free(thirdOperandCOLSTR);
	free(secondBracketCOLSTR);

	return expCOLSTR;
}

std::string TernaryExpression::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append("(");
	coloredString.append(firstOperand->getExpressionCOLSTR());
	coloredString.append(" ? ");
	coloredString.append(secondOperand->getExpressionCOLSTR());
	coloredString.append(" : ");
	coloredString.append(thirdOperand->getExpressionCOLSTR());
	coloredString.append(")");
	return coloredString;
}

std::string TernaryExpression::to_json(){
	std::string json_string = "{\"expression_type\":\"TernaryExpression\", ";
	json_string.append("\"firstOperand\":");
	json_string.append(firstOperand->to_json() + ", ");
	json_string.append("\"secondOperand\":");
	json_string.append(secondOperand->to_json() + ", ");
	json_string.append("\"thirdOperand\":");
	json_string.append(thirdOperand->to_json() + "}");
	return json_string;
}

//std::vector<Expression*>* TernaryExpression::getExpressionElements(bool includePointers){
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
//	std::vector<Expression*>* thirdOperandElements = thirdOperand->getExpressionElements(includePointers);
//	expElements->insert(expElements->end(), thirdOperandElements->begin(), thirdOperandElements->end());
//	delete thirdOperandElements;
//
//	return expElements;
//}

void TernaryExpression::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	createExpressionElements(firstOperand, includePointers, elements_out);
	createExpressionElements(secondOperand, includePointers, elements_out);
	createExpressionElements(thirdOperand, includePointers, elements_out);
}

ExpressionPtr TernaryExpression::deepcopy(){
	TernaryExpressionPtr copyPtr = std::make_shared<TernaryExpression>(firstOperand->deepcopy(), secondOperand->deepcopy(), thirdOperand->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}


void TernaryExpression::replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	//if(nameToBeReplaced.compare(firstOperand->getName()) == 0  && subscriptToBeReplaced == firstOperand->subscript){
	//	delete firstOperand;
	//	firstOperand = replacingExpression;
	//}
	//else{
	firstOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
	//}

	if(!applyLocalSimplifications(secondOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(secondOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete secondOperand;
			secondOperand = replacingExpression->deepcopy();
		}
		else{
			secondOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}

	if(!applyLocalSimplifications(thirdOperand, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(thirdOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete thirdOperand;
			thirdOperand = replacingExpression->deepcopy();
		}
		else{
			thirdOperand->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
}

bool TernaryExpression::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool firstOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, firstOperand);
	bool secondOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, secondOperand);
	bool thirdOperandReplaced = replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, thirdOperand);
	return firstOperandReplaced || secondOperandReplaced || thirdOperandReplaced;
}

bool TernaryExpression::equals(ExpressionPtr toThisExpression){
	if(type == toThisExpression->type){
		TernaryExpressionPtr toThisTernaryExpression = std::dynamic_pointer_cast<TernaryExpression>(toThisExpression);
		if(firstOperand->equals(toThisTernaryExpression->firstOperand) &&
			secondOperand->equals(toThisTernaryExpression->secondOperand) &&
			thirdOperand->equals(toThisTernaryExpression->thirdOperand)){
			return true;
		}
	}
	return false;
}

ExpressionPtr TernaryExpression::simplifyExpression(){
	ExpressionPtr simplifiedExpression = std::shared_ptr<Expression>();
	if(firstOperand->type == HIGHLEVEL_CONDITION){
		HighLevelConditionPtr hlCondition = std::dynamic_pointer_cast<HighLevelCondition>(firstOperand);
		simplifiedExpression = hlCondition->isConditionTrue() ? secondOperand->deepcopy() :
								hlCondition->isConditionFalse() ? thirdOperand->deepcopy() : NULL;
	}
	return simplifiedExpression;
}
