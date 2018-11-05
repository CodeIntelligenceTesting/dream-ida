/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class ADDFlagMacro :
	public FlagMacro
{
public:
	ADDFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result);
	virtual ~ADDFlagMacro(void);

	std::string getInstructionString();
	std::string to_json();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	//std::vector<Flag> definedFlags;
	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	ExpressionPtr result;
};

