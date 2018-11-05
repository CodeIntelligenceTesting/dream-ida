/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include <map>

#include "Expression.h"
#include "Flag.h"

class FlagConcatenation :
	public Expression
{
public:
	FlagConcatenation(intToFlagMapPtr _flagMap);
	virtual ~FlagConcatenation(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();

	intToFlagMapPtr flagMap;
};

