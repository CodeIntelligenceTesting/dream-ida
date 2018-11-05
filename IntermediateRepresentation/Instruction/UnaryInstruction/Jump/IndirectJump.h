/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "Jump.h"

class IndirectJump :
	public Jump
{
public:
	IndirectJump(ExpressionPtr _targetBlock, InstructionType _instructionType);
	virtual ~IndirectJump(void);

	std::string getTargetString();
	char* getTargetColoredString();
	std::string getTargetCOLSTR();
	//std::vector<Expression*>* getUsedElements();
	//std::vector<Expression*>* getDefinedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();

	ExpressionPtr targetBlock;
};

