////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "N_WayNode.h"


N_WayNode::N_WayNode(int _id, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions, intSetPtr _successorIDs) : Node(_id, N_WAY_NODE, _startEA, _endEA, _instructions)
{
	successorIDs = _successorIDs;
}


N_WayNode::~N_WayNode(void)
{
	//delete successorIDs;
}

NodePtr N_WayNode::deepCopy(){
	intSetPtr successorIDsCopy = std::make_shared<std::set<int>>();
	for(std::set<int>::iterator succ_iter = successorIDs->begin() ; succ_iter != successorIDs->end() ; ++succ_iter)
		successorIDsCopy->insert(*succ_iter);
	return std::make_shared<N_WayNode>(id, startEA, endEA, getInstructionsCopy(), successorIDsCopy);
}
