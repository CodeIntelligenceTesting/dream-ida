/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "IndirectJump.h"
#include <map>

class Expression;

class IndirectTableJump :
	public IndirectJump
{
public:
	IndirectTableJump(ExpressionPtr _targetBlock, ExpressionPtr _jumpTableIndex, ExpressionPtr _switchVariable, nodeToCasesMapPtr _nodeToCasesMap, int _defaultBlockId);
	virtual ~IndirectTableJump(void);

	ExpressionPtr jumpTableIndex;
	ExpressionPtr switchVariable;
	nodeToCasesMapPtr nodeToCasesMap;
	int defaultBlockId;
	int startAddressOfSwitchIdiom;

	std::string getInstructionString();
	std::string getInstructionCOLSTR();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	InstructionPtr deepcopy();
};

