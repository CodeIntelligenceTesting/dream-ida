////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "OperandSimplificationInfo.h"


OperandSimplificationInfo::OperandSimplificationInfo(Expression* _operandExpression, double _coefficient)
{
	operandExpression = _operandExpression;
	coefficient = _coefficient;
}


OperandSimplificationInfo::~OperandSimplificationInfo(void)
{
}
