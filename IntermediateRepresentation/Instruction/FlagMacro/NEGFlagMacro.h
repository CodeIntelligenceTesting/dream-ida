/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class NEGFlagMacro :
	public FlagMacro
{
public:
	NEGFlagMacro(ExpressionPtr _operand);
	virtual ~NEGFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	//std::vector<Flag> definedFlags;
	ExpressionPtr operand;
};

