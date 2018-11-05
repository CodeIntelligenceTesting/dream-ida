/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "BinaryNonCommutativeExpression.h"

class DivisionExpression :
	public BinaryNonCommutativeExpression
{
public:
	DivisionExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand);
	virtual ~DivisionExpression(void);

	ExpressionPtr deepcopy();
	std::string getOperationString();
	std::string to_json();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
};

