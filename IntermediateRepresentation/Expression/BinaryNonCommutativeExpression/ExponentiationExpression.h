/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "BinaryNonCommutativeExpression.h"

class ExponentiationExpression :
	public BinaryNonCommutativeExpression
{
public:
	ExponentiationExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand);
	virtual ~ExponentiationExpression(void);

	ExpressionPtr deepcopy();
	ExpressionPtr simplifyExpression();
	std::string getOperationString();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);

	std::string to_json();
};

