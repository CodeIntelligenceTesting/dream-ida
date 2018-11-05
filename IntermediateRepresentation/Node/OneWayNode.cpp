////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "OneWayNode.h"


OneWayNode::OneWayNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, int _successorID): Node(_id, ONE_WAY_NODE, _startEA, _endEA, _instructions)
{
	successorID = _successorID;
}


OneWayNode::~OneWayNode(void)
{
}


intSetPtr OneWayNode::getSuccessors(){
	intSetPtr successors = std::make_shared<std::set<int>>();
	successors->insert(successorID);
	return successors;
}

NodePtr OneWayNode::deepCopy(){
	return std::make_shared<OneWayNode>(id, startEA, endEA, getInstructionsCopy(), successorID);
}
