/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../Expression.h"

class Constant :
	public Expression
{
public:
	Constant(ExpressionType _expressionType);
	virtual ~Constant(void);

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();

	//virtual std::string getValueAsString() = 0;
};

