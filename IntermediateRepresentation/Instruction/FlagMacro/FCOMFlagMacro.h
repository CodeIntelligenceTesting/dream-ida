/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class FCOMFlagMacro :
	public FlagMacro
{
public:
	FCOMFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, bool _setEFLAGS);
	virtual ~FCOMFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	bool setEFLAGS;
};

