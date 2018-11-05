/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "../TypeBase.h"

class SimpleType :
	public TypeBase
{
public:
	SimpleType(TypeEmun _type, std::string _typeString = "");
	virtual ~SimpleType(void);
};

