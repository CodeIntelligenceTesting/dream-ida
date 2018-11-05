/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Constant.h"
#include "../../Enumerations/SymbolicConstantValues.h"

class SymbolicConstant :
	public Constant
{
public:
	SymbolicConstant(SymbolicConstantValue _value);
	virtual ~SymbolicConstant(void);

	ExpressionPtr deepcopy();
	bool equals(ExpressionPtr toThisExpression);

	std::string getExpressionString();
	TypePtr getExpresstionType(TypeMapPtr typeMap);

	std::string to_json();

	SymbolicConstantValue value;
};

