/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class LogicalFlagMacro :
	public FlagMacro
{
public:
	LogicalFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result);
	virtual ~LogicalFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();
	std::string to_json();

	//std::vector<Flag> definedFlags;
	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	ExpressionPtr result;
};

