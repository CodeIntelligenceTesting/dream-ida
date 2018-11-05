/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "ComposedType.h"

class UnionType :
	public ComposedType
{
public:
	UnionType(std::string _typeString = "");
	virtual ~UnionType(void);

	std::string getTypeCOLSTR(){return "";};
};
