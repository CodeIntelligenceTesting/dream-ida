/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once


#include "../Instruction/Instruction.h"
#include "../Expression/ExpressionTypes.h"

class DefinitionInfo
{
public:
	DefinitionInfo(void);
	virtual ~DefinitionInfo(void);

	Instruction* definingInstruction;
	ExpressionType definedVariableType;
};

