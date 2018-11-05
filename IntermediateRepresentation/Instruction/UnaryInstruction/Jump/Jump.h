/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include "../UnaryInstruction.h"

class Jump :
	public UnaryInstruction
{
public:
	Jump(InstructionType _instructionType);
	~Jump(void);

	std::string getInstructionString();
	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	//std::vector<Expression*>* getUsedElements();
	//std::vector<Expression*>* getDefinedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out){};
//protected:
	virtual std::string getTargetString() = 0;
	virtual char* getTargetColoredString() = 0;
	virtual std::string getTargetCOLSTR() = 0;
	//std::string to_json();
};
