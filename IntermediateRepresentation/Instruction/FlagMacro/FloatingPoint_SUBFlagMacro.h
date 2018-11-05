/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class FloatingPoint_SUBFlagMacro :
	public FlagMacro
{
public:
	FloatingPoint_SUBFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result);
	~FloatingPoint_SUBFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	//std::vector<Flag> definedFlags;
	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	ExpressionPtr result;
};

