/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../IntermediateRepresentation/IRSharedPointers.h"

class TrivialNodesEliminator
{
public:
	TrivialNodesEliminator(ControlFlowGraphPtr _controlFlowGraph);
	virtual ~TrivialNodesEliminator(void);

	void eliminateTrivialNodes();
	//void removeNode(ControlFlowGraphPtr &controlFlowGraph, int nodeId);
private:
	ControlFlowGraphPtr controlFlowGraph;

	OneWayNodePtr getAnEmptyOneWayNode();
	void removeDirectJumpInstructions();
	void updateSuccessorsOfPredecessorNodes(const OneWayNodePtr &emptyOneWayNode);

	//bool hasOnlyDirectJumpInstruction(NodePtr node);
	//void getEmptyNodes(const ControlFlowGraphPtr &controlFlowGraph, std::set<int> &emptyNodes_out);
	//void updateSuccessorsOfPredecessorNodes(ControlFlowGraphPtr &controlFlowGraph, const OneWayNodePtr &emptyOneWayNode);
	
	//void updateNodeEdges(NodePtr &node, int removedNodeId);
	//void updateLastInstructionIfBranch(NodePtr &node);
	//ControlFlowGraphPtr controlFlowGraph;
};

