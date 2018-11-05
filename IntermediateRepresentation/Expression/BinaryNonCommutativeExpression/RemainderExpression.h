/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "BinaryNonCommutativeExpression.h"
#include "../../../TypeAnalysis/Types/SimpleType/IntegralType.h"

class RemainderExpression :
	public BinaryNonCommutativeExpression
{
public:
	RemainderExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand);
	virtual ~RemainderExpression(void);

	ExpressionPtr deepcopy();
	std::string getOperationString();
	std::string to_json();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
};

