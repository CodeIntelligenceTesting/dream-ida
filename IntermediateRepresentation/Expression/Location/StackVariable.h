/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

#include "Location.h"

class StackVariable :
	public Location
{
public:
	StackVariable(std::string _name);
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string getName();
	std::string to_json();

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	virtual ~StackVariable(void);

	std::string getExpressionString();
	ExpressionPtr deepcopy();
};

