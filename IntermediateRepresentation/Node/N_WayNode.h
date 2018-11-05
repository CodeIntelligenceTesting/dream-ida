/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Node.h"

class N_WayNode :
	public Node
{
public:
	N_WayNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, intSetPtr _successorIDs);
	virtual ~N_WayNode(void);

	intSetPtr getSuccessors(){return successorIDs;};
	NodePtr deepCopy();

	intSetPtr successorIDs;
};

