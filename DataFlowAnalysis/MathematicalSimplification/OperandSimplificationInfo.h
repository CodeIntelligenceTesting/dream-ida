/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/Expression/Expression.h"

class OperandSimplificationInfo
{
public:
	OperandSimplificationInfo(Expression* _operandExpression, double _coefficient);
	virtual ~OperandSimplificationInfo(void);
	
	Expression*  operandExpression;
	double coefficient;
};

