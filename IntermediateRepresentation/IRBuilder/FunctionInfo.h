/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <pro.h>
#include <typeinf.hpp>


enum type_status{
	TYPE_FAILED,
	TYPE_OK,
	TYPE_GUESSED
};

class FunctionInfo
{
public:
	FunctionInfo(ea_t addr);
	virtual ~FunctionInfo(void);
	qstring name;
	qstring type;
	cm_t callingConvention;
	int argNum;
	bool isVoid;
	type_status status;
};

