/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

enum InstructionType{
	ASSIGNMENT,
	CALL,
	CONDITIONAL_JUMP,
	FLAG_MACRO,
	DIRECT_JUMP,
	INDIRECT_JUMP,
	INDIRECT_TABLE_JUMP,
	PHI_FUNCTION,
	RETURN,
	SWITCH
};
