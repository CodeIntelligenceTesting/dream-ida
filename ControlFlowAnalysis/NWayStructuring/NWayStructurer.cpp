////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "NWayStructurer.h"


#include "../GraphUtilities/GraphConverter.h"
#include "../GraphTraversal/GraphPaths.h"
#include "../GraphTraversal/GraphTraversal.h"


#include "../../IntermediateRepresentation/ControlFlowGraph.h"
#include "../../IntermediateRepresentation/Node/NodeTypes.h"

#include <boost/graph/dominator_tree.hpp>

#define NO_NODE -1

NWayStructurer::NWayStructurer(ControlFlowGraphPtr _controlFlowGraph)
{
	controlFlowGraph = _controlFlowGraph;
	GraphConverter graphConverter;
	graphConverter.convertToBoostGraph(controlFlowGraph, graph);
	buildDominatorTree();
	GraphTraversal graphTraversal(graph);
	postorder = graphTraversal.postorder;
}


NWayStructurer::~NWayStructurer(void)
{
}


void NWayStructurer::structureNWay(GraphStructeringInfoPtr &graphStructeringInfo){
	std::set<int> unresolvedNodesIds;
	for(std::vector<int>::iterator node_iter = postorder->begin() ; node_iter != postorder->end() ; ++node_iter){
		NodePtr currentNode = controlFlowGraph->nodes->at(*node_iter);
		if(currentNode->nodeType == N_WAY_NODE){
			int succNotImmediatelyDominatedId = findSuccessorNotImmediatelyDominatedByHeader(currentNode->id);
			NodePtr n = currentNode;
			if(succNotImmediatelyDominatedId != NO_NODE){
				//Find common immediate dominator of successors
			}
			else{
				n = currentNode;
			}
			int followNodeId = findFollowNodeId(n->id);
			graphStructeringInfo->switches.insert(std::pair<int, NWaySwitch>(n->id, NWaySwitch(n->id, followNodeId)));
			//msg("<SWITCH> %d -> %d\n", n->id, followNodeId);
		}
	}
}

void NWayStructurer::buildDominatorTree(){
	std::vector<Vertex> domTreePredVector;
	IndexMap indexMap = boost::get(boost::vertex_index, graph);

	VertexIterator iter, iterEnd;
	
	domTreePredVector = std::vector<Vertex>(boost::num_vertices(graph), boost::graph_traits<Graph>::null_vertex());
	PredMap domTreePredMap = boost::make_iterator_property_map(domTreePredVector.begin(), indexMap);
	boost::lengauer_tarjan_dominator_tree(graph, boost::vertex(0, graph), domTreePredMap);

	for(boost::tie(iter, iterEnd) = boost::vertices(graph) ; iter != iterEnd ; ++iter){
		if(boost::get(domTreePredMap,*iter) != boost::graph_traits<Graph>::null_vertex()){
			int dominatingNode = indexMap[boost::get(domTreePredMap, *iter)];
			int dominatedNode = indexMap[*iter];
			dom_tree[dominatingNode].insert(dominatedNode);
		}
	}
}

int NWayStructurer::findSuccessorNotImmediatelyDominatedByHeader(int headerId){
	intSetPtr successors = controlFlowGraph->getSuccessors(headerId);
	for(std::set<int>::iterator succ_iter = successors->begin() ; succ_iter != successors->end() ; ++succ_iter){
		std::set<int>* immediatelyDominatedNodes = &(dom_tree[headerId]);
		if(immediatelyDominatedNodes->find(*succ_iter) == immediatelyDominatedNodes->end())
			return *succ_iter;
	}
	return NO_NODE;
}

int NWayStructurer::findFollowNodeId(int headerId){
	std::set<int>* immediatelyDominatedNodes = &(dom_tree[headerId]);
	int maxInEdges = 0, followNodeId = NO_NODE;
	for(std::set<int>::iterator idom_iter = immediatelyDominatedNodes->begin() ; idom_iter != immediatelyDominatedNodes->end() ; ++idom_iter){
		int currentImmediatelyDominatedNodeId = *idom_iter;
		int numInEdges = controlFlowGraph->getPredecessors(currentImmediatelyDominatedNodeId)->size();
		if( numInEdges >= std::max(2, maxInEdges)){
			maxInEdges = numInEdges;
			followNodeId = currentImmediatelyDominatedNodeId;
		}
	}

	if(followNodeId == NO_NODE){
		intSetPtr headerPredecessors = controlFlowGraph->getPredecessors(headerId);
		if(headerPredecessors->size() == 1)
			return findFollowNodeId(*(headerPredecessors->begin()));
		else
			return followNodeId;
	}
	return followNodeId;
}
