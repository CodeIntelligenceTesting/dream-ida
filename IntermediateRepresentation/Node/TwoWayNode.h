/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Node.h"

class TwoWayNode :
	public Node
{
public:
	TwoWayNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, int _trueSuccessorID, int _falseSuccessorID);
	virtual ~TwoWayNode(void);

	intSetPtr getSuccessors();
	NodePtr deepCopy();

	int trueSuccessorID;
	int falseSuccessorID;
};

