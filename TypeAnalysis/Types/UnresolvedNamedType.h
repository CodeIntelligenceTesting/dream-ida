/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "TypeBase.h"

class UnresolvedNamedType :
	public TypeBase
{
public:
	UnresolvedNamedType(std::string _typeString);
	virtual ~UnresolvedNamedType(void);

	std::string getTypeCOLSTR(){return typeString;};
	bool equals(TypePtr comparedType);
	TypePtr deepcopy();
};

