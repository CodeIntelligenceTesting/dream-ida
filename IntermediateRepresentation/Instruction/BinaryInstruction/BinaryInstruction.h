/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../Instruction.h"
#include "../InstructionTypes.h" 
#include "../../Expression/Expression.h"

class BinaryInstruction :
	public Instruction
{
public:
	BinaryInstruction(InstructionType _type);
	virtual ~BinaryInstruction(void);
};

