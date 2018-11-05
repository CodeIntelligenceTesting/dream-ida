/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <string>

#include "TypeSharedPointers.h"

class TypeMap
{
public:
	TypeMap(void);
	virtual ~TypeMap(void);

	void startNewAnalysisRound();
	bool shouldPerformNewAnalysisRound();

	TypePtr getType(const std::string& variableName, int variableSubscript);
	void setType(const std::string& variableName, int variableSubscript, TypePtr type);

//TODO uncomment
//private:
	std::map<std::string, std::map<int, TypePtr>> types;
	bool newTypesResolved;
};

