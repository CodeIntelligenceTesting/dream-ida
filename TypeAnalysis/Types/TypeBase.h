/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "TypeTypes.h"
#include "../TypeSharedPointers.h"

class TypeBase
{
public:
	TypeBase(TypeEmun _type ,std::string _typeString = "");
	virtual ~TypeBase(void);

	virtual std::string getTypeCOLSTR() = 0/*{return typeString;}*/;

	std::string typeString;
	TypeEmun type;
	bool isConst;

	std::string getTypeFromSize(int size_in_bytes);

	virtual bool isIntegerType(){return false;};
	virtual bool isFloatType(){return false;};
	virtual bool isPointerType(){return false;};
	virtual bool isVoidType(){return false;};

	virtual bool equals(TypePtr comparedType) = 0;

	virtual TypePtr deepcopy() = 0;
};

