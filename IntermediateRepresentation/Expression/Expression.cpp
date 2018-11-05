////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Expression.h"
#include "UnaryExpression/NegationExpression.h"
//#include "../../Shared/ExpressionUtilities.h"

#include "../../TypeAnalysis/Types/TopType.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralTypes.h"

Expression::Expression(ExpressionType _type, int _subscript){
	type = _type;
	subscript = _subscript;
	phiSourceBlockId = NO_BLOCK_ID;
	size_in_bytes = 4;
	expressionType = std::make_shared<TopType>();
}


Expression::~Expression(void)
{
}


bool Expression::doesExpressionUseAPointer(){
	bool expressionUsesAPointer = false;
	std::vector<ExpressionPtr> elements;
	
	getExpressionElements(true, elements);

	for(std::vector<ExpressionPtr>::iterator element_iter = elements.begin() ; element_iter != elements.end() ; ++element_iter){
		if((*element_iter)->type == POINTER){
			return true;
		}
	}
	return false;
}


bool Expression::equals(const ExpressionPtr& toThisExpression){
	if(type == toThisExpression->type){
		return getExpressionString().compare(toThisExpression->getExpressionString()) == 0;
	}
	return false;
}


bool Expression::isSameVariable(const std::string& variableName, int variableSubscript){
	std::string name = getName();
	return !name.empty() && variableName.compare(name) == 0 && subscript != NO_SUBSCRIPT && subscript == variableSubscript;
}

bool Expression::replaceOperandNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression, ExpressionPtr& operandPtr){
	if(operandPtr->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
		operandPtr = replacingExpression->deepcopy();
		return true;
	}
	return operandPtr->replaceChildExpressionNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
}

void Expression::replaceBySimplifiedExpression(ExpressionPtr& expressionToSimplifyPtr){
	ExpressionPtr simplifiedExpressionPtr = expressionToSimplifyPtr->simplifyExpression();
	if(simplifiedExpressionPtr != std::shared_ptr<Expression>() && simplifiedExpressionPtr != expressionToSimplifyPtr){
		expressionToSimplifyPtr = simplifiedExpressionPtr;
	}
}

bool Expression::isMemoryVariable(){
	return type == POINTER || type == GLOBAL_VARIABLE;
}


std::string Expression::expressionTypeToString(){
	return expressionType->type != TOP_TYPE && expressionType->type != NO_TYPE 
			? expressionType->getTypeCOLSTR() 
			: expressionType->getTypeFromSize(size_in_bytes);
}

void Expression::set_expression_size(int byte_size){
	this->size_in_bytes = byte_size;
	if(byte_size == 1){
		this->expressionType = std::make_shared<IntegralType>(CHAR_TYPE);
	}
	else if(byte_size == 2){
		this->expressionType = std::make_shared<IntegralType>(SHORT_TYPE);
	}
}

//TypePtr Expression::getExpresstionType(DefinitionResolverPtr definitionResolver){
//
//	/*std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->find(getName());
//	if(name_iter != definitionsMap->end()){
//		std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.find(subscript);
//		if(subscript_iter != name_iter->second.end()){
//			ExpressionPtr expressionDefinition = subscript_iter->second->getDefinedExpression(name_iter->first, subscript_iter->first);
//			if(expressionDefinition.get() != NULL){
//				return expressionDefinition->expressionType;
//			}
//		}
//	}*/
//	ExpressionPtr expressionDefinition = definitionResolver->getExpressionDefinition(getName(), subscript);
//	if(expressionDefinition.get() != NULL){
//		return expressionDefinition->expressionType;
//	}
//		
//	return expressionType;
//}
