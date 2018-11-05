/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "SimpleType.h"

#include "../../TypeSharedPointers.h"

class PointerType :
	public SimpleType
{
public:
	PointerType(std::string _typeString = "");
	virtual ~PointerType(void);
	std::string getTypeCOLSTR();

	bool isPointerType(){return true;};
	bool equals(TypePtr comparedType);
	TypePtr deepcopy();
//private:
	TypePtr pointedToType;
};
