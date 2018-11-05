/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "ComposedType.h"

class ClassType :
	public ComposedType
{
public:
	ClassType(std::string _typeString = "");
	virtual ~ClassType(void);

	std::string getTypeCOLSTR(){return "";};
};

