////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Condition.h"


Condition::Condition(ExpressionType _expressionType, int _subscript) : Expression(_expressionType, _subscript)
{
	//subscript = -1;
}


Condition::~Condition(void)
{
}
