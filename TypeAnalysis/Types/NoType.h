/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "TypeBase.h"
class NoType :
	public TypeBase
{
public:
	NoType(void);
	virtual ~NoType(void);

	std::string getTypeCOLSTR(){return "";};
	bool equals(TypePtr comparedType);

	TypePtr deepcopy();
};

