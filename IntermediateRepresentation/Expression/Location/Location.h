/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../Expression.h"
#include "../ExpressionTypes.h"

class Location :
	public Expression
{
public:
	Location(ExpressionType _expressionType, std::string _name, int _subscript);
	~Location(void);

	std::string name;
	//int subscript;
};

