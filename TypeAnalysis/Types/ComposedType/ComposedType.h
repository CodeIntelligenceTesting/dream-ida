/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "../TypeBase.h"

class ComposedType :
	public TypeBase
{
public:
	ComposedType(TypeEmun _type, std::string _typeString = "");
	virtual ~ComposedType(void);
};

