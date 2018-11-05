/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <intel.hpp>

#include <string>
#include <set>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "Expression.h"
#include "ExpressionTypes.h"
#include "../Enumerations/RegisterTypes.h"

class Register :
	public Expression
{
public:
	Register(uint16 _regNo, char _dtyp = -1);
	virtual ~Register(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string getName();
	std::string to_json();

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	ExpressionPtr deepcopy();

	uint16 regNo;
	char dtyp;
	//int subscript;
	RegisterType registerType;
	std::string name;
	std::set<std::string> relatedRegistersNames;
};

