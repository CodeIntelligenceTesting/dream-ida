/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class RORFlagMacro :
	public FlagMacro
{
public:
	RORFlagMacro(ExpressionPtr _operand, ExpressionPtr _count, ExpressionPtr _result);
	virtual ~RORFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	//std::vector<Flag> definedFlags;
	ExpressionPtr operand;
	ExpressionPtr count;
	ExpressionPtr result;
};

