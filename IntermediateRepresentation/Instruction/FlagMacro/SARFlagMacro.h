/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class SARFlagMacro :
	public FlagMacro
{
public:
	SARFlagMacro(ExpressionPtr _operand, ExpressionPtr _count, ExpressionPtr _result);
	virtual ~SARFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	//std::vector<Flag> definedFlags;
	ExpressionPtr operand;
	ExpressionPtr count;
	ExpressionPtr result;
};

