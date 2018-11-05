////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DefinitionResolver.h"

#include "IRSharedPointers.h"
#include "Expression/Expression.h"
#include "Instruction/Instruction.h"

DefinitionResolver::DefinitionResolver(definitionsMapPtr _definitionsMap)
{
	definitionsMap = _definitionsMap;
}


DefinitionResolver::~DefinitionResolver(void)
{
}


//TypePtr TypeMap::getExpressionType(ExpressionPtr expression){
//	ExpressionPtr expressionDefinition = getExpressionDefinition(expression);
//	if(expressionDefinition.get() != NULL){
//		return expressionDefinition->getExpresstionType();
//	}
//	return std::make_shared<TopType>();
//}
//
//void TypeMap::setExpressionType(ExpressionPtr expression, TypePtr expressionType){
//	ExpressionPtr expressionDefinition = getExpressionDefinition(expression);
//	if(expressionDefinition.get() != NULL){
//		expressionDefinition->expressionType = expressionType;
//	}
//}

ExpressionPtr DefinitionResolver::getExpressionDefinition(const std::string name, int subscript){
	std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->find(name);
	if(name_iter != definitionsMap->end()){
		std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.find(subscript);
		if(subscript_iter != name_iter->second.end()){
			return subscript_iter->second->getDefinedExpression(name_iter->first, subscript_iter->first);
		}
	}
	return std::shared_ptr<Expression>();
}
