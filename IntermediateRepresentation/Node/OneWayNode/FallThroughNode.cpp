////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "FallThroughNode.h"


FallThroughNode::FallThroughNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, int _successorID) : OneWayNode(_id, _startEA, _endEA, _instructions, _successorID)
{
}


FallThroughNode::~FallThroughNode(void)
{
}

NodePtr FallThroughNode::deepCopy(){
	return std::make_shared<FallThroughNode>(id, startEA, endEA, getInstructionsCopy(), successorID);
}
