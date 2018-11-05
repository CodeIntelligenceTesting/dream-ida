/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "BinaryNonCommutativeExpression.h"

class ShiftExpression :
	public BinaryNonCommutativeExpression
{
public:
	ShiftExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, bool _isRight);
	virtual ~ShiftExpression(void);

	ExpressionPtr deepcopy();
	std::string getOperationString();
	std::string to_json();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);

private:
	bool isRight;
};

