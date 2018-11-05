/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

#include "Expression.h"
#include "ExpressionTypes.h"

class LocalVariable :
	public Expression
{
public:
	LocalVariable(std::string _name);
	~LocalVariable(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string getName();
	std::string to_json();

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	ExpressionPtr deepcopy();

	std::string name;
	//int subscript;
};

