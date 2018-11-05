////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ControlFlowAnalyzer.h"
#include "GraphStructeringInfo.h"
#include "TrivialNodesEliminator.h"
#include "CompoundConditionalStructurer.h"
#include "GraphTraversal/GraphTraversal.h"
#include "DerivedGraphSequence/DerivedGraphSequenceConstructor.h"

#include "NWayStructuring/NWayStructurer.h"
#include "LoopStructuring/LoopStructurer.h"
#include "ConditionalsStructuring/ConditionalStructurer.h"

#include "GraphUtilities/GraphConverter.h"
#include "../IntermediateRepresentation/ControlFlowGraph.h"

ControlFlowAnalyzer::ControlFlowAnalyzer(ControlFlowGraphPtr _controlFlowGraph)
{
	controlFlowGraph = _controlFlowGraph;
	graphStructeringInfo = std::make_shared<GraphStructeringInfo>();
}


ControlFlowAnalyzer::~ControlFlowAnalyzer(void)
{
}

void ControlFlowAnalyzer::applyControlFlowAnalysis(){
	//removeDirectJumpInstructions();
	TrivialNodesEliminator trivialNodesEliminator(controlFlowGraph);
	trivialNodesEliminator.eliminateTrivialNodes();

	CompoundConditionalStructurer compoundConditionalStructurer(controlFlowGraph);
	compoundConditionalStructurer.structureCompoundConditionals();

	NWayStructurer nWayStructurer(controlFlowGraph);
	nWayStructurer.structureNWay(graphStructeringInfo);
	LoopStructurer loopStructurer(controlFlowGraph);
	loopStructurer.structureLoops(graphStructeringInfo);
	ConditionalStructurer conditionalStructurer(controlFlowGraph, getHeaderAndLatchingNodes());
	conditionalStructurer.structureConditionals(graphStructeringInfo);

	/*for(std::map<int, Conditional>::iterator it = graphStructeringInfo->conditionals.begin() ; it != graphStructeringInfo->conditionals.end() ; ++it){
		msg("Conditional--> Header(%d), Follow(%d)\n", it->second.headerNodeId, it->second.followNodeId);
	}*/
}


intSetPtr ControlFlowAnalyzer::getHeaderAndLatchingNodes(){
	intSetPtr headerLatchingNodes = std::make_shared<std::set<int>>();
	for(std::map<int, Loop>::iterator loop_iter = graphStructeringInfo->loops.begin() ; loop_iter != graphStructeringInfo->loops.end() ; ++loop_iter){
		headerLatchingNodes->insert(loop_iter->second.headerNodeId);
		headerLatchingNodes->insert(loop_iter->second.latchingNodeId);
	}
	return headerLatchingNodes;
}

void ControlFlowAnalyzer::removeDirectJumpInstructions(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		InstructionVectorPtr instructions = node_iter->second->instructions;
		if(!instructions->empty() && instructions->back()->type == DIRECT_JUMP)
			instructions->pop_back();
	}
}
