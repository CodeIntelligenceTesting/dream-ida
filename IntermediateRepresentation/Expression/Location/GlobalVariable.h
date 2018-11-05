/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

#include <pro.h>

#include "Location.h"

class GlobalVariable :
	public Location
{
public:
	GlobalVariable(std::string _name, ea_t _address);
	virtual ~GlobalVariable(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string getName();
	std::string to_json();

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	ExpressionPtr deepcopy();

	/*bool equals(Expression* toThisExpression);
	bool equalsDiscardConstant(Expression* toThisExpression);*/

	ea_t address;
};

