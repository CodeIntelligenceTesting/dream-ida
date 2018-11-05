////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Location.h"


Location::Location(ExpressionType _expressionType, std::string _name, int _subscript) : Expression(_expressionType, _subscript)
{
	name = _name;
	//subscript = -1;
}


Location::~Location(void)
{
}
