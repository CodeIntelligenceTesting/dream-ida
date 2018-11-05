////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TwoWayNode.h"


TwoWayNode::TwoWayNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, int _trueSuccessorID, int _falseSuccessorID) : Node(_id, TWO_WAY_NODE, _startEA, _endEA, _instructions)
{
	trueSuccessorID =_trueSuccessorID;
	falseSuccessorID = _falseSuccessorID;
}


TwoWayNode::~TwoWayNode(void)
{
}


intSetPtr TwoWayNode::getSuccessors(){
	intSetPtr successors = std::make_shared<std::set<int>>();
	successors->insert(trueSuccessorID);
	successors->insert(falseSuccessorID);
	return successors;
}

NodePtr TwoWayNode::deepCopy(){
	return std::make_shared<TwoWayNode>(id, startEA, endEA, getInstructionsCopy(), trueSuccessorID, falseSuccessorID);
}
