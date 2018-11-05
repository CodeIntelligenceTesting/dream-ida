/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Constant.h"

#define MIN_POINTER_VALUE 256

class NumericConstant :
	public Constant
{
public:
	NumericConstant(int _value);
	NumericConstant(int _value, bool _isSigned);
	virtual ~NumericConstant(void);

	ExpressionPtr deepcopy();
	bool equals(ExpressionPtr toThisExpression);

	std::string getExpressionString();
	TypePtr getExpresstionType(TypeMapPtr typeMap);

	std::string to_json();

	int value;
	bool isSigned;
};

