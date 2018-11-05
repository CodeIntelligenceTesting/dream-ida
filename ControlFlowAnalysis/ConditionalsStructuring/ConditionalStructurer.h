/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "ConditionalTypes.h"
#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "../GraphStructeringInfo.h"
#include "../GraphTypes.h"

class ConditionalStructurer
{
public:
	ConditionalStructurer(ControlFlowGraphPtr _controlFlowGraph, intSetPtr _headerLatchingNodes);
	virtual ~ConditionalStructurer(void);

	void structureConditionals(GraphStructeringInfoPtr &graphStructeringInfo);
private:
	ControlFlowGraphPtr controlFlowGraph;
	intSetPtr headerLatchingNodes;
	Graph graph;
	std::map<int, std::set<int>> dom_tree;
	intVectorPtr reversePostorder;

	void buildDominatorTree();

	int getImmediatelyDominatedNodeWithMaxReversePostorder(int dominatingNodeId);
	ConditionalType getConditionalType(int headerNodeId, int followNodeId);
	void handleUnresolvedConditionalsInLoops(GraphStructeringInfoPtr &graphStructeringInfo, std::set<int> &unresolvedNodesIds);
	//bool getPathVertices(Vertex start, Vertex end, std::vector<int> &pathVertices);
	//int getCommonNodeId(const std::vector<int> &firstNodeVector, const std::vector<int> &secondNodeVector);

	bool isHeaderNodeOfPretestedLoop(int nodeId, const std::map<int, Loop>& structuredLoops);
	bool isLatchingNodeOfPosttestedLoop(int nodeId, const std::map<int, Loop>& structuredLoops);
};

