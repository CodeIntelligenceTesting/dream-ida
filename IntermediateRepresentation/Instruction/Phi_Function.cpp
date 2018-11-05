////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Phi_Function.h"
#include <boost/lexical_cast.hpp>
#include "../../Shared/ExpressionUtilities.h"


Phi_Function::Phi_Function(ExpressionPtr _target, int argNum) : Instruction(PHI_FUNCTION)
{
	target = _target;
	arguments = std::make_shared<std::vector<ExpressionPtr>>();
	for(int i = 0 ; i < argNum ; i++){
		arguments->push_back(target->deepcopy());
	}
}

Phi_Function::~Phi_Function(void)
{
	//msg("deleting.. %s\n", getInstructionString().c_str());
	/*delete target; target = NULL;
	deleteVectorPointer(arguments); arguments = NULL;*/
}

std::string Phi_Function::getInstructionString(){
	std::string instString = target->getExpressionString() + "[" + boost::lexical_cast<std::string>(target->phiSourceBlockId) + "]" + " = Phi(";
	for(std::vector<ExpressionPtr>::iterator arg_iter = arguments->begin() ; arg_iter != arguments->end() ; ++arg_iter){
		instString += (*arg_iter)->getExpressionString() + "[" + boost::lexical_cast<std::string>((*arg_iter)->phiSourceBlockId) + "]";
		if(arg_iter + 1 != arguments->end()){
			instString += ", ";
		}
	}
	instString += ")";
	return instString;
}

char* Phi_Function::getColoredInstructionString(){
	return getColoredString(getInstructionString(), MACRO_COLOR);
}

std::string Phi_Function::getInstructionCOLSTR(){
	return getCOLSTR(getInstructionString(), MACRO_COLOR);
}

//std::vector<Expression*>* Phi_Function::getUsedElements(){
//	std::vector<Expression*>* usedElements = new std::vector<Expression*>();
//	for(std::vector<Expression*>::iterator iter = arguments->begin() ; iter != arguments->end() ; ++iter){
//		std::vector<Expression*>* paramElements = (*iter)->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), paramElements->begin(), paramElements->end());
//		delete paramElements;
//	}
//	if(target->type == POINTER){
//		std::vector<Expression*>* addressElements = target->getExpressionElements(false);
//		usedElements->insert(usedElements->end() , addressElements->begin(), addressElements->end());
//		delete addressElements;
//	}
//	return usedElements;
//}
//
//std::vector<Expression*>* Phi_Function::getDefinedElements(){
//	if(target->type == POINTER){
//		std::vector<Expression*>* definedElements = new std::vector<Expression*>();
//		definedElements->push_back(target);
//		return definedElements;
//	}
//	else{
//		return target->getExpressionElements(true);
//	}
//}

void Phi_Function::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	for(std::vector<ExpressionPtr>::iterator iter = arguments->begin() ; iter != arguments->end() ; ++iter){
		createExpressionElements(*iter, true, usedElements_out);
	}
	if(target->type == POINTER)
		createExpressionElements(target, false, usedElements_out);
}

void Phi_Function::getDefinedElements(std::vector<ExpressionPtr>& definedElements_out){
	if(target->type == POINTER)
		definedElements_out.push_back(target);
	else
		createExpressionElements(target, true, definedElements_out);
}

bool Phi_Function::replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(target->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
		//delete target;
		int phiBlock = target->phiSourceBlockId;
		target = replacingExpression->deepcopy();
		target->phiSourceBlockId = phiBlock;
		return true;
	}
	return false;
}

bool Phi_Function::replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool result = false;
	for(std::vector<ExpressionPtr>::iterator arg_iter = arguments->begin() ; arg_iter != arguments->end() ; ++arg_iter){
		if((*arg_iter)->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete *arg_iter;
			int phiBlock = (*arg_iter)->phiSourceBlockId;
			*arg_iter = replacingExpression->deepcopy();
			(*arg_iter)->phiSourceBlockId = phiBlock;
			result = true;
		}
	}
	return result;
}

InstructionPtr Phi_Function::deepcopy(){
	ExpressionPtr targetCopy = target->deepcopy();
	return std::make_shared<Phi_Function>(targetCopy, arguments->size());
}
