////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Instruction.h"


Instruction::Instruction(InstructionType _type){
	type = _type;
	ContainingNodeId = -1;
}


Instruction::~Instruction(void)
{
}

//std::vector<Expression*>* Instruction::getAllElements(){
//	std::vector<Expression*>* elements = getUsedElements();
//	std::vector<Expression*>* definedElements = getDefinedElements();
//	if(definedElements != NULL){
//		elements->insert(elements->end(), definedElements->begin(), definedElements->end());
//		delete definedElements;
//	}
//	return elements;
//}

void Instruction::getAllElements(std::vector<ExpressionPtr>& elements_out){
	getDefinedElements(elements_out);
	getUsedElements(elements_out);
}

ExpressionPtr Instruction::getDefinedExpression(const std::string& name, int subscript){
	std::vector<ExpressionPtr> definedExpressions;
	getDefinedElements(definedExpressions);
	return getExpressionFromVector(name, subscript, definedExpressions);
}

void Instruction::getUsesOfExpression(const std::string& name, int subscript, std::vector<ExpressionPtr>& uses){
	if(!name.empty() && subscript != NO_SUBSCRIPT){
		std::vector<ExpressionPtr> usedExpressions;
		getUsedElements(usedExpressions);
		for(std::vector<ExpressionPtr>::iterator usedExp_iter = usedExpressions.begin() ; usedExp_iter != usedExpressions.end() ; ++usedExp_iter){
			ExpressionPtr usedExpression = *usedExp_iter;
			if(name.compare(usedExpression->getName()) == 0 && subscript == usedExpression->subscript){
				uses.push_back(usedExpression);
			}
		}
	}
}


bool Instruction::isOverwritingStatement(){
	std::vector<ExpressionPtr> definedElements, usedElements;
	getDefinedElements(definedElements);
	getUsedElements(usedElements);
	if(!definedElements.empty() && !usedElements.empty()){
		for(std::vector<ExpressionPtr>::iterator def_iter = definedElements.begin() ; def_iter != definedElements.end() ; ++def_iter){
			for(std::vector<ExpressionPtr>::iterator use_iter = usedElements.begin() ; use_iter != usedElements.end() ; ++use_iter){
				std::string defName = (*def_iter)->getName(), useName = (*use_iter)->getName();
				if(defName.compare(useName) == 0 && !defName.empty() && !useName.empty())
					return true;
			}
		}
	}
	return false;
}

void Instruction::replaceBySimplifiedExpression(ExpressionPtr& expressionToSimplifyPtr){
	ExpressionPtr simplifiedExpressionPtr = expressionToSimplifyPtr->simplifyExpression();
	if(simplifiedExpressionPtr.get() != NULL && simplifiedExpressionPtr != expressionToSimplifyPtr){
		expressionToSimplifyPtr = simplifiedExpressionPtr;
	}
}

unsigned int Instruction::getNumberOfUsedExpressions(){
	std::vector<ExpressionPtr> usedElements;
	getUsedElements(usedElements);
	return usedElements.size();
}

ExpressionPtr Instruction::getExpressionFromVector(const std::string& name, int subscript, std::vector<ExpressionPtr>& expressionsVector){
	for(std::vector<ExpressionPtr>::iterator def_iter = expressionsVector.begin() ; def_iter != expressionsVector.end() ; ++def_iter){
		ExpressionPtr exprtession = *def_iter;
		if(name.compare(exprtession->getName()) == 0 && subscript == exprtession->subscript
			&& !name.empty() && subscript != NO_SUBSCRIPT){
				return exprtession;
		}
	}
	ExpressionPtr nullExpression = std::shared_ptr<Expression>();
	return nullExpression;
}
