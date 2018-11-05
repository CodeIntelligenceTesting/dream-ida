/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../Expression.h"
#include "../ExpressionTypes.h"

class Condition :
	public Expression
{
public:
	Condition(ExpressionType _expressionType, int _subscript);
	virtual ~Condition(void);

	//int subscript;
};

