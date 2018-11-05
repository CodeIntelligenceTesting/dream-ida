/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class SALFlagMacro :
	public FlagMacro
{
public:
	SALFlagMacro(ExpressionPtr _operand, ExpressionPtr _count, ExpressionPtr _result);
	virtual ~SALFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	//std::vector<Flag> definedFlags;
	ExpressionPtr operand;
	ExpressionPtr count;
	ExpressionPtr result;
};

