////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TypeMap.h"

#include "Types/TopType.h"

#include <idp.hpp>

TypeMap::TypeMap(void)
{
	newTypesResolved = true;
}


TypeMap::~TypeMap(void)
{
}

void TypeMap::startNewAnalysisRound(){
	newTypesResolved = false;
}

bool TypeMap::shouldPerformNewAnalysisRound(){
	return newTypesResolved;
}

TypePtr TypeMap::getType(const std::string& variableName, int variableSubscript){
	std::map<std::string, std::map<int, TypePtr>>::iterator name_iter = types.find(variableName);
	if(name_iter != types.end()){
		std::map<int, TypePtr>::iterator subscript_iter = name_iter->second.find(variableSubscript);
		if(subscript_iter != name_iter->second.end())
			return subscript_iter->second;
	}
	return std::make_shared<TopType>();
}

void TypeMap::setType(const std::string& variableName, int variableSubscript, TypePtr type){
	if(!variableName.empty()){
		TypePtr oldType = getType(variableName, variableSubscript);
		if(!type->isVoidType() && !oldType->equals(type)){
			/*msg("(%s_%d)***** oldType(%s) %s newType(%s) *****\n",
				variableName.c_str(), variableSubscript,
				oldType->getTypeCOLSTR().c_str(),
				type->equals(oldType) ? "==" : "!=",
				type->getTypeCOLSTR().c_str()
				);*/
			newTypesResolved = true;
			types[variableName][variableSubscript] = type;
		}
	}
}
