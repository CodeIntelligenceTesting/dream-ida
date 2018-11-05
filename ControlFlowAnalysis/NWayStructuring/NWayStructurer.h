/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "../GraphStructeringInfo.h"
#include "../GraphTypes.h"

class NWayStructurer
{
public:
	NWayStructurer(ControlFlowGraphPtr _controlFlowGraph);
	virtual ~NWayStructurer(void);

	void structureNWay(GraphStructeringInfoPtr &graphStructeringInfo);
private:
	ControlFlowGraphPtr controlFlowGraph;
	Graph graph;
	std::map<int, std::set<int>> dom_tree;
	intVectorPtr postorder;

	void buildDominatorTree();
	int findSuccessorNotImmediatelyDominatedByHeader(int headerId);
	int findFollowNodeId(int headerId);
};

