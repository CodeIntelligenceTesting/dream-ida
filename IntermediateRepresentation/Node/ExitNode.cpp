////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ExitNode.h"


ExitNode::ExitNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions) : Node(_id, Exit_NODE, _startEA, _endEA, _instructions)
{
}


ExitNode::~ExitNode(void)
{
}

NodePtr ExitNode::deepCopy(){
	return std::make_shared<ExitNode>(id, startEA, endEA, getInstructionsCopy());
}
