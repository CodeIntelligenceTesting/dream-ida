/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "Jump.h"
class DirectJump :
	public Jump
{
public:
	DirectJump(int _targetBlockId);
	virtual ~DirectJump(void);

	std::string getTargetString();
	char* getTargetColoredString();
	std::string getTargetCOLSTR();

	InstructionPtr deepcopy();
	//std::vector<Expression*>* getUsedElements(){return NULL;};
	//std::vector<Expression*>* getDefinedElements(){return NULL;};

	int targetBlockId;
};

