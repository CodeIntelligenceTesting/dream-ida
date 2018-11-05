/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "../GraphTypes.h"

//typedef std::pair<int, int> Edge;

class GraphTraversal
{
public:
	GraphTraversal(const Graph &cfg);
	virtual ~GraphTraversal(void);

	void applyDepthFirstSearch();

	intVectorPtr preorder;
	intVectorPtr postorder;

private:
	//ControlFlowGraphPtr controlFlowGraph;
	//void constructGraphEdges(std::vector<Edge> &graphEdges_out);
};

