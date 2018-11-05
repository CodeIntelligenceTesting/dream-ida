/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../IntermediateRepresentation/IRSharedPointers.h"
#include "GraphTypes.h"

class ControlFlowAnalyzer
{
public:
	ControlFlowAnalyzer(ControlFlowGraphPtr _controlFlowGraph);
	virtual ~ControlFlowAnalyzer(void);

	void applyControlFlowAnalysis();
	GraphStructeringInfoPtr graphStructeringInfo;
private:
	ControlFlowGraphPtr controlFlowGraph;

	intSetPtr getHeaderAndLatchingNodes();
	void removeDirectJumpInstructions();
};
