////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Call.h"
#include "../../Shared/ExpressionUtilities.h"
//#include "../../Shared/houseKeepingUtilities.h"

Call::Call(ExpressionPtr _functionPointer, ExpressionVectorPtr _parameters, ExpressionVectorPtr _returns) : Instruction(CALL)
{
	functionPointer = _functionPointer;
	parameters = _parameters;
	returns = _returns;
}

Call::~Call(void)
{
	/*delete functionPointer; functionPointer = NULL;
	deleteVectorPointer(parameters); parameters = NULL;
	deleteVectorPointer(returns); returns = NULL;*/
}

std::string Call::getInstructionString(){
	std::string callString;
	if(returns->size() > 0){
		std::vector<ExpressionPtr>::iterator iter;
		for(iter = returns->begin() ; iter != returns->end() ; iter++){
			callString += (*iter)->expressionType->getTypeCOLSTR() + (*iter)->getExpressionString();
			if(iter + 1 != returns->end()){
				callString += ", ";
			}
		}
		callString += " = ";
	}
	callString += functionPointer->getExpressionString();
	if(parameters->size() > 0){
		std::vector<ExpressionPtr>::iterator iter;
		callString += "(";
		for(iter = parameters->begin() ; iter != parameters->end() ; iter++){
			callString += (*iter)->getExpressionString();
			if(iter + 1 != parameters->end()){
				callString += ", ";
			}
		}
		callString += ")";
	}
	else{
		callString += "()";
	}
	return callString;
}

char* Call::getColoredInstructionString(){
	char** returnsCOLSTR = NULL;
	char** parametersCOLSTR = NULL;
	char* commaCOLSTR = getColoredString(", ", CODE_COLOR);;
	char* equalCOLSTR = getColoredString(" = ", CODE_COLOR);;
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	char* functionNameCOLSTR = getColoredString(functionPointer->getExpressionString(), FUNCTION_COLOR);/*functionPointer->getColoredExpressionString();*/

	int* returnsCOLSTRSize = NULL;
	int* parametersCOLSTRSize = NULL;
	int commaCOLSTRSize = strlen(commaCOLSTR);
	int equalCOLSTRSize = strlen(equalCOLSTR);
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);
	int functionNameCOLSTRSize = strlen(functionNameCOLSTR);

	int instCOLSTRSize = functionNameCOLSTRSize + firstBracketCOLSTRSize + secondBracketCOLSTRSize;
	if(returns->size() > 0){
		returnsCOLSTR = new char*[returns->size()];
		returnsCOLSTRSize = new int[returns->size()];
		for(unsigned int i = 0 ; i < returns->size() ; i++){
			returnsCOLSTR[i] = (*returns)[i]->getColoredExpressionString();
			returnsCOLSTRSize[i] = strlen(returnsCOLSTR[i]);
			instCOLSTRSize += returnsCOLSTRSize[i];
			if(i != returns->size() - 1){
				instCOLSTRSize += commaCOLSTRSize;
			}
		}
		instCOLSTRSize += equalCOLSTRSize;
	}
	if(parameters->size() > 0){
		parametersCOLSTR = new char*[parameters->size()];
		parametersCOLSTRSize = new int[parameters->size()];
		for(unsigned int i = 0 ; i < parameters->size() ; i++){
			parametersCOLSTR[i] = (*parameters)[i]->getColoredExpressionString();
			parametersCOLSTRSize[i] = strlen(parametersCOLSTR[i]);
			instCOLSTRSize += parametersCOLSTRSize[i];
			if(i != parameters->size() - 1){
				instCOLSTRSize += commaCOLSTRSize;
			}
		}
	}

	char* instCOLSTR = (char*)malloc(instCOLSTRSize + 1);
	int currentPosition = 0;
	if(returnsCOLSTR != NULL){
		for(unsigned int i = 0 ; i < returns->size() ; i++){
			memcpy(instCOLSTR + currentPosition, returnsCOLSTR[i], returnsCOLSTRSize[i]);
			currentPosition += returnsCOLSTRSize[i];
			if(i != returns->size() - 1){
				memcpy(instCOLSTR + currentPosition, commaCOLSTR, commaCOLSTRSize);
				currentPosition += commaCOLSTRSize;
			}
			free(returnsCOLSTR[i]);
		}
		memcpy(instCOLSTR + currentPosition, equalCOLSTR, equalCOLSTRSize);
		currentPosition += equalCOLSTRSize;
	}

	memcpy(instCOLSTR + currentPosition, functionNameCOLSTR, functionNameCOLSTRSize);
	currentPosition += functionNameCOLSTRSize;

	memcpy(instCOLSTR + currentPosition, firstBracketCOLSTR, firstBracketCOLSTRSize);
	currentPosition += firstBracketCOLSTRSize;

	if(parametersCOLSTR != NULL){
		for(unsigned int i = 0 ; i < parameters->size() ; i++){
			memcpy(instCOLSTR + currentPosition, parametersCOLSTR[i], parametersCOLSTRSize[i]);
			currentPosition += parametersCOLSTRSize[i];
			if(i != parameters->size() - 1){
				memcpy(instCOLSTR + currentPosition, commaCOLSTR, commaCOLSTRSize);
				currentPosition += commaCOLSTRSize;
			}
			free(parametersCOLSTR[i]);
		}
	}

	memcpy(instCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);
	currentPosition += secondBracketCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, "\0", 1);

	free(functionNameCOLSTR);
	free(firstBracketCOLSTR);
	free(secondBracketCOLSTR);
	free(commaCOLSTR);
	free(equalCOLSTR);

	delete[] returnsCOLSTR;
	delete[] returnsCOLSTRSize;
	delete[] parametersCOLSTR;
	delete[] parametersCOLSTRSize;

	return instCOLSTR;
}

std::string Call::getInstructionCOLSTR(){
	std::string coloredString;
	if(!returns->empty()){
		std::vector<ExpressionPtr>::iterator iter;
		for(iter = returns->begin() ; iter != returns->end() ; iter++){
			coloredString.append(/*(*iter)->expressionType->getTypeCOLSTR() +*/ (*iter)->getExpressionCOLSTR());
			if(iter + 1 != returns->end()){
				coloredString.append(", ");
			}
		}
		coloredString.append(" = ");
	}

	coloredString.append(getCOLSTR(functionPointer->getExpressionString(), FUNCTION_COLOR));
	if(!parameters->empty()){
		std::vector<ExpressionPtr>::iterator iter;
		coloredString.append("(");
		for(iter = parameters->begin() ; iter != parameters->end() ; iter++){
			coloredString.append((*iter)->getExpressionCOLSTR());
			if(iter + 1 != parameters->end()){
				coloredString.append(", ");
			}
		}
		coloredString.append(")");
	}
	else{
		coloredString.append("()");
	}
	return coloredString;
}

std::string Call::to_json(){
	std::string json_string = "{\"instruction_type\":\"CALL\", ";
	json_string.append("\"functionPointer\":" + functionPointer->to_json() + ", \"returns\":[");
	if(!returns->empty()){
		std::vector<ExpressionPtr>::iterator iter;
		for(iter = returns->begin() ; iter != returns->end() ; iter++){
			json_string.append((*iter)->to_json());
			if(iter + 1 != returns->end()){
				json_string.append(", ");
			}
		}
	}
	json_string.append("], \"parameters\":[");

	if(!parameters->empty()){
		std::vector<ExpressionPtr>::iterator iter;
		for(iter = parameters->begin() ; iter != parameters->end() ; iter++){
			json_string.append((*iter)->to_json());
			if(iter + 1 != parameters->end()){
				json_string.append(", ");
			}
		}
	}
	json_string.append("]}");
	return json_string;
}

//std::vector<Expression*>* Call::getUsedElements(){
//	std::vector<Expression*>* usedElements = new std::vector<Expression*>();
//	usedElements->push_back(functionPointer);
//	for(std::vector<Expression*>::iterator iter = parameters->begin() ; iter != parameters->end() ; ++iter){
//		std::vector<Expression*>* paramElements = (*iter)->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), paramElements->begin(), paramElements->end());
//		delete paramElements;
//	}
//	for(std::vector<Expression*>::iterator iter = returns->begin() ; iter != returns->end() ; ++iter){
//		if((*iter)->type == POINTER){
//			std::vector<Expression*>* retElements = (*iter)->getExpressionElements(false);
//			usedElements->insert(usedElements->end(), retElements->begin(), retElements->end());
//			delete retElements;
//		}
//	}
//	return usedElements;
//}
//
//std::vector<Expression*>* Call::getDefinedElements(){
//	std::vector<Expression*>* definedElements = new std::vector<Expression*>();
//	for(std::vector<Expression*>::iterator iter = returns->begin() ; iter != returns->end() ; ++iter){
//		if((*iter)->type == POINTER){
//			definedElements->push_back(*iter);
//		}
//		else{
//			std::vector<Expression*>* retElements = (*iter)->getExpressionElements(true);
//			definedElements->insert(definedElements->end(), retElements->begin(), retElements->end());
//			delete retElements;
//		}
//	}
//	return definedElements;
//}

void Call::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	usedElements_out.push_back(functionPointer);
	for(std::vector<ExpressionPtr>::iterator iter = parameters->begin() ; iter != parameters->end() ; ++iter)
		createExpressionElements(*iter, true, usedElements_out);
	for(std::vector<ExpressionPtr>::iterator iter = returns->begin() ; iter != returns->end() ; ++iter){
		if((*iter)->type == POINTER)
			createExpressionElements(*iter, false, usedElements_out);
	}
}

void Call::getDefinedElements(std::vector<ExpressionPtr>& definedElements_out){
	for(std::vector<ExpressionPtr>::iterator iter = returns->begin() ; iter != returns->end() ; ++iter){
		if((*iter)->type == POINTER)
			definedElements_out.push_back(*iter);
		else
			createExpressionElements(*iter, true, definedElements_out);
	}
}

void Call::replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	for(std::vector<ExpressionPtr>::iterator param_iter = parameters->begin() ; param_iter != parameters->end() ; ++param_iter){
		ExpressionPtr paramExp = *param_iter;
		//msg("before %s\n", (*param_iter)->getExpressionString().c_str());
		//msg("applyLocalSimplifications(%s, %s, %d) = %d\n", paramExp->getExpressionString().c_str(), nameToBeReplaced.c_str(), subscriptToBeReplaced, applyLocalSimplifications(&(*param_iter), nameToBeReplaced, subscriptToBeReplaced, replacingExpression));
		if(true/*!applyLocalSimplifications(*param_iter, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)*/){
			//msg("after %s\n", (*param_iter)->getExpressionString().c_str());
			if(paramExp->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
				//msg("replaced\n");
				//delete paramExp;
				*param_iter = replacingExpression->deepcopy();
			}
			else{
				//msg("paramExp->replaceChildExpression(%s)\n", replacingExpression->getExpressionString().c_str());
				//msg("delegated\n");
				paramExp->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
			}
			//msg("before: %s\n", (*param_iter)->getExpressionString().c_str());
			//Expression* simpExp = (*param_iter)->simplifyExpression();
			//msg("after:  %s\n", (simpExp != NULL ? simpExp : (*param_iter))->getExpressionString().c_str());
			replaceBySimplifiedExpression(*param_iter);
		}
	}
}

bool Call::replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	return replaceElementInList(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, returns);
}

bool Call::replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	return replaceElementInList(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, parameters);
}

bool Call::replaceElementInList(std::string nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression, ExpressionVectorPtr& containingList){
	bool result = false;
	for(std::vector<ExpressionPtr>::iterator element_iter = containingList->begin() ; element_iter != containingList->end() ; ++element_iter){
		ExpressionPtr element = *element_iter;
		if(element->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete *element_iter;
			*element_iter = replacingExpression->deepcopy();
			result = true;
		}
		else{
			result = element->replaceChildExpressionNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
	return result;
}

InstructionPtr Call::deepcopy(){
	ExpressionPtr functionPointerCopy = functionPointer->deepcopy();
	
	ExpressionVectorPtr parametersCopy = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator param_iter = parameters->begin() ; param_iter != parameters->end() ; ++param_iter){
		parametersCopy->push_back((*param_iter)->deepcopy());
	}

	ExpressionVectorPtr returnsCopy = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator ret_iter = returns->begin() ; ret_iter != returns->end() ; ++ret_iter){
		returnsCopy->push_back((*ret_iter)->deepcopy());
	}

	return std::make_shared<Call>(functionPointerCopy, parametersCopy, returnsCopy);
}
