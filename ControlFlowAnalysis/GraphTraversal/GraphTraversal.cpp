////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "GraphTraversal.h"

#include "DFSOrderVisitor.h"
#include "../../IntermediateRepresentation/ControlFlowGraph.h"
#include "../GraphTypes.h"

//typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
//typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;


GraphTraversal::GraphTraversal(const Graph &cfg)
{
	DFSOrderVisitor dfsOrderVisitor;
	boost::depth_first_search(cfg, boost::visitor(dfsOrderVisitor));

	preorder = dfsOrderVisitor.preorder;
	postorder = dfsOrderVisitor.postorder;
}


GraphTraversal::~GraphTraversal(void)
{
}

void GraphTraversal::applyDepthFirstSearch(){
	/*std::vector<Edge> graphEdges;
	constructGraphEdges(graphEdges);

	Graph cfg(graphEdges.begin(), graphEdges.end(), controlFlowGraph->nodes->size());
	DFSOrderVisitor dfsOrderVisitor;

	boost::depth_first_search(cfg, boost::visitor(dfsOrderVisitor));

	preorder = dfsOrderVisitor.preorder;
	postorder = dfsOrderVisitor.postorder;*/
}

//void GraphTraversal::constructGraphEdges(std::vector<Edge> &graphEdges_out){
//	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
//		NodePtr currentNode = node_iter->second;
//		if(currentNode->nodeType == ONE_WAY_NODE){
//			OneWayNodePtr currentOneWayNode = std::dynamic_pointer_cast<OneWayNode>(currentNode);
//			graphEdges_out.push_back(Edge(currentOneWayNode->id, currentOneWayNode->successorID));
//		}
//		else if(currentNode->nodeType == TWO_WAY_NODE){
//			TwoWayNodePtr currentTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(currentNode);
//			graphEdges_out.push_back(Edge(currentTwoWayNode->id, currentTwoWayNode->trueSuccessorID));
//			graphEdges_out.push_back(Edge(currentTwoWayNode->id, currentTwoWayNode->falseSuccessorID));
//		}
//		else if(currentNode->nodeType == N_WAY_NODE){
//			N_WayNodePtr currentNWayNode = std::dynamic_pointer_cast<N_WayNode>(currentNode);
//			for(std::vector<int>::iterator successor_it = currentNWayNode->successorIDs->begin() ; successor_it != currentNWayNode->successorIDs->end() ; ++successor_it){
//				graphEdges_out.push_back(Edge(currentNWayNode->id, *successor_it));
//			}
//		}
//	}
//}
