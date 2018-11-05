/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

//#include <ida.hpp>
#include <idp.hpp>
//#include <loader.hpp>

#include "Expression.h"
#include "ExpressionTypes.h"
#include "../Enumerations/Flags.h"

class Flag :
	public Expression
{
public:
	Flag(FlagType _flagType, FlagNo _flagNo);
	virtual ~Flag(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string getName();

	//void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();
	std::string to_json();

	FlagType flagType;
	FlagNo flagNo;
	//int subscript;
	std::string name;
};

