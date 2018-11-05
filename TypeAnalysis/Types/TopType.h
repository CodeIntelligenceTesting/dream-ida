/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "TypeBase.h"
class TopType :
	public TypeBase
{
public:
	TopType(void);
	virtual ~TopType(void);

	std::string getTypeCOLSTR();

	bool isVoidType(){return true;};
	bool equals(TypePtr comparedType);
	TypePtr deepcopy();
};

