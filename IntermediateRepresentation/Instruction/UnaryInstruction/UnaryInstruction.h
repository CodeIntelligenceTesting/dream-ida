/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../Instruction.h"
#include "../InstructionTypes.h"

class UnaryInstruction :
	public Instruction
{
public:
	UnaryInstruction(InstructionType _type);
	virtual ~UnaryInstruction(void);
};

