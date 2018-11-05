////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ReturnNode.h"


ReturnNode::ReturnNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions) : Node(_id, RETURN_NODE, _startEA, _endEA, _instructions)
{
}


ReturnNode::~ReturnNode(void)
{
}


NodePtr ReturnNode::deepCopy(){
	return std::make_shared<ReturnNode>(id, startEA, endEA, getInstructionsCopy());
}
