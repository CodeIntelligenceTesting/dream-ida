/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Node.h"

class ExitNode :
	public Node
{
public:
	ExitNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions);
	virtual ~ExitNode(void);

	NodePtr deepCopy();
};

