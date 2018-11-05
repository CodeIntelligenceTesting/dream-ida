////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "GraphConverter.h"

#include "../../IntermediateRepresentation/ControlFlowGraph.h"


GraphConverter::GraphConverter(void)
{
}


GraphConverter::~GraphConverter(void)
{
}


void GraphConverter::convertToBoostGraph(ControlFlowGraphPtr controlFlowGraph, Graph &boostGraph){
	std::vector<Edge> graphEdges;
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		if(currentNode->nodeType == ONE_WAY_NODE){
			OneWayNodePtr currentOneWayNode = std::dynamic_pointer_cast<OneWayNode>(currentNode);
			graphEdges.push_back(Edge(currentOneWayNode->id, currentOneWayNode->successorID));
		}
		else if(currentNode->nodeType == TWO_WAY_NODE){
			TwoWayNodePtr currentTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(currentNode);
			graphEdges.push_back(Edge(currentTwoWayNode->id, currentTwoWayNode->trueSuccessorID));
			graphEdges.push_back(Edge(currentTwoWayNode->id, currentTwoWayNode->falseSuccessorID));
		}
		else if(currentNode->nodeType == N_WAY_NODE){
			N_WayNodePtr currentNWayNode = std::dynamic_pointer_cast<N_WayNode>(currentNode);
			for(std::set<int>::iterator successor_it = currentNWayNode->successorIDs->begin() ; successor_it != currentNWayNode->successorIDs->end() ; ++successor_it){
				graphEdges.push_back(Edge(currentNWayNode->id, *successor_it));
			}
		}
	}
	boostGraph = Graph(graphEdges.begin(), graphEdges.end(), controlFlowGraph->nodes->size());
}
