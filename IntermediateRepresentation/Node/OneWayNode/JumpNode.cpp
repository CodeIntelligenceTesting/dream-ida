////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "JumpNode.h"


JumpNode::JumpNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, int _successorID) : OneWayNode(_id, _startEA, _endEA, _instructions, _successorID)
{
}


JumpNode::~JumpNode(void)
{
}

NodePtr JumpNode::deepCopy(){
	return std::make_shared<JumpNode>(id, startEA, endEA, getInstructionsCopy(), successorID);
}
