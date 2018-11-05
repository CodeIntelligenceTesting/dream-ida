/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "ComposedType.h"

#include "../../TypeSharedPointers.h"

class ArrayType :
	public ComposedType
{
public:
	ArrayType(void);
	virtual ~ArrayType(void);

	std::string getTypeCOLSTR(){return "";};

	TypePtr elementType;
	int numberOfElements;
};

