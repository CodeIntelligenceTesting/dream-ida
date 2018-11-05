////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ConditionalStructurer.h"


#include "../GraphUtilities/GraphConverter.h"
#include "../GraphTraversal/GraphPaths.h"
#include "../GraphTraversal/VertexOrdering.h"

#include "../../IntermediateRepresentation/ControlFlowGraph.h"

#include <boost/graph/dominator_tree.hpp>

ConditionalStructurer::ConditionalStructurer(ControlFlowGraphPtr _controlFlowGraph, intSetPtr _headerLatchingNodes)
{
	controlFlowGraph = _controlFlowGraph;
	headerLatchingNodes = _headerLatchingNodes;
	GraphConverter graphConverter;
	graphConverter.convertToBoostGraph(controlFlowGraph, graph);
	buildDominatorTree();
	reversePostorder = getReversePostorder(graph);
}

ConditionalStructurer::~ConditionalStructurer(void)
{
}

void ConditionalStructurer::structureConditionals(GraphStructeringInfoPtr &graphStructeringInfo){
	std::set<int> unresolvedNodesIds;
	for(std::vector<int>::reverse_iterator nodeId_iter = reversePostorder->rbegin() ; nodeId_iter != reversePostorder->rend() ; ++nodeId_iter){
		int nodeId = *nodeId_iter;
		NodePtr currentNode = controlFlowGraph->nodes->at(nodeId);
		if(currentNode->nodeType == TWO_WAY_NODE
			&& !isHeaderNodeOfPretestedLoop(currentNode->id, graphStructeringInfo->loops)
			&& !isLatchingNodeOfPosttestedLoop(currentNode->id, graphStructeringInfo->loops) /*headerLatchingNodes->find(nodeId) == headerLatchingNodes->end()*/){
				int followNodeId = getImmediatelyDominatedNodeWithMaxReversePostorder(nodeId);
				if(followNodeId == -1){
					unresolvedNodesIds.insert(nodeId);
				}
				else{
					ConditionalType conditionalType = getConditionalType(nodeId, followNodeId);
					graphStructeringInfo->conditionals.insert(std::pair<int, Conditional>(nodeId, Conditional(nodeId, followNodeId, conditionalType)));
					for(std::set<int>::iterator unresolvedNodeId_iter = unresolvedNodesIds.begin() ; unresolvedNodeId_iter != unresolvedNodesIds.end() ; ++unresolvedNodeId_iter){
						int unresolvedHeaderId = *unresolvedNodeId_iter;
						if(unresolvedHeaderId >= followNodeId){
							graphStructeringInfo->conditionals.insert(std::pair<int, Conditional>(unresolvedHeaderId, Conditional(unresolvedHeaderId, -1, IF_THEN_ELSE)));
						}
						else{
							conditionalType = getConditionalType(unresolvedHeaderId, followNodeId);
							graphStructeringInfo->conditionals.insert(std::pair<int, Conditional>(unresolvedHeaderId, Conditional(unresolvedHeaderId, followNodeId, conditionalType)));
						}
					}
					unresolvedNodesIds.clear();
				}
		}
	}
	handleUnresolvedConditionalsInLoops(graphStructeringInfo, unresolvedNodesIds);
	
	for(std::set<int>::iterator unresolvedNodeId_iter = unresolvedNodesIds.begin() ; unresolvedNodeId_iter != unresolvedNodesIds.end() ; ++unresolvedNodeId_iter)
		graphStructeringInfo->conditionals.insert(std::pair<int, Conditional>(*unresolvedNodeId_iter, Conditional(*unresolvedNodeId_iter, -1, UNKNOWN_CONDITIONAL)));

	/*msg("Conditionals:\n");
	for(std::map<int, Conditional>::iterator it = graphStructeringInfo->conditionals.begin() ; it != graphStructeringInfo->conditionals.end() ; ++it)
		msg("Header: %d\nFollow: %d\nType: %d\n-----\n", it->second.headerNodeId, it->second.followNodeId, it->second.conditionalType);*/
}

void ConditionalStructurer::buildDominatorTree(){
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

int ConditionalStructurer::getImmediatelyDominatedNodeWithMaxReversePostorder(int dominatingNodeId){
	int result = -1;
	int resultReversePostorder = /*-1*/controlFlowGraph->nodes->size() + 1;
	for(std::set<int>::iterator dominatedNodeId_iter = dom_tree[dominatingNodeId].begin() ; dominatedNodeId_iter != dom_tree[dominatingNodeId].end() ; ++dominatedNodeId_iter){
		int dominatedNodeId = *dominatedNodeId_iter;
		Vertex dominatedNode = boost::vertex(dominatedNodeId, graph);
		if(boost::in_degree(dominatedNode, graph) >= 2){
			int currentReversePostorder = getReversePostorderNumber(dominatedNodeId, reversePostorder);
			if(currentReversePostorder /*>*/< resultReversePostorder){
				result = dominatedNodeId;
				resultReversePostorder = currentReversePostorder;
			}
		}
	}
	return result;
}

ConditionalType ConditionalStructurer::getConditionalType(int headerNodeId, int followNodeId){
	Vertex headerNode = boost::vertex(headerNodeId, graph), followNode = boost::vertex(followNodeId, graph);
	OutEdgeIterator e, end;
	for(boost::tie(e, end) = boost::out_edges(headerNode, graph) ; e != end ; ++e){
		Vertex successor = boost::target(*e, graph);
		if(successor == followNode)
			return IF_THEN;
	}
	return IF_THEN_ELSE;
}

void ConditionalStructurer::handleUnresolvedConditionalsInLoops(GraphStructeringInfoPtr &graphStructeringInfo, std::set<int> &unresolvedNodesIds){
	std::set<int>::iterator unresolvedNodeId_iter = unresolvedNodesIds.begin();
	bool isConditionalStructured = false;
	while(unresolvedNodeId_iter != unresolvedNodesIds.end()){
		int unresolvedNodeId = *unresolvedNodeId_iter;
		for(std::map<int, Loop>::iterator loop_iter = graphStructeringInfo->loops.begin() ; loop_iter != graphStructeringInfo->loops.end() ; ++loop_iter){
			if(loop_iter->second.loopNodes->find(unresolvedNodeId) != loop_iter->second.loopNodes->end()){
				Vertex loopHeader = boost::vertex(loop_iter->second.headerNodeId, graph);
				TwoWayNodePtr unresolvedBasicBlock = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(unresolvedNodeId));
				Vertex trueSuccessor = boost::vertex(unresolvedBasicBlock->trueSuccessorID, graph);
				Vertex falseSuccessor = boost::vertex(unresolvedBasicBlock->falseSuccessorID, graph);
				
				std::vector<int> trueSuccToHeadPathVertices, falseSuccToHeadPathVertices;

				if(getPathVertices(trueSuccessor, loopHeader, trueSuccToHeadPathVertices, graph)
					&& getPathVertices(falseSuccessor, loopHeader, falseSuccToHeadPathVertices, graph)){
						int commonNodeId = getCommonNodeId(trueSuccToHeadPathVertices, falseSuccToHeadPathVertices);
						ConditionalType conditionalType = IF_THEN_ELSE;
						if(commonNodeId != -1)
							conditionalType = getConditionalType(unresolvedNodeId, commonNodeId);
						graphStructeringInfo->conditionals.insert(std::pair<int, Conditional>(unresolvedNodeId, Conditional(unresolvedNodeId, commonNodeId, conditionalType)));
						unresolvedNodeId_iter = unresolvedNodesIds.erase(unresolvedNodeId_iter);
						isConditionalStructured = true;
						break;
				}
			}
		}

		if(!isConditionalStructured)
			++unresolvedNodeId_iter;
		else
			isConditionalStructured = false;
	}
}

//bool ConditionalStructurer::getPathVertices(Vertex start, Vertex end, std::vector<int> &pathVertices){
//	int numOfVertices = boost::num_vertices(graph);
//	std::vector<Vertex> p(numOfVertices);
//	std::vector<int> d(numOfVertices);
//	boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&p[0]).distance_map(&d[0]));
//	if(d[end] != (std::numeric_limits<int>::max)()){
//		IndexMap index = boost::get(boost::vertex_index, graph);
//		Vertex target = end;
//		while(target != start){
//			pathVertices.push_back(index[target]);
//			target = p[target];
//		}
//		pathVertices.push_back(index[target]);
//		return true;
//	}
//	return false;
//}
//
//int ConditionalStructurer::getCommonNodeId(const std::vector<int> &firstNodeVector, const std::vector<int> &secondNodeVector){
//	for(std::vector<int>::const_reverse_iterator nodeId_iter = firstNodeVector.rbegin() ; nodeId_iter != firstNodeVector.rend() ; ++nodeId_iter){
//		if(std::find(secondNodeVector.begin() , secondNodeVector.end(), *nodeId_iter) != secondNodeVector.end()){
//			return *nodeId_iter;
//		}
//	}
//	return -1;
//}


bool ConditionalStructurer::isHeaderNodeOfPretestedLoop(int nodeId, const std::map<int, Loop>& structuredLoops){
	for(std::map<int, Loop>::const_iterator loop_iter = structuredLoops.begin() ; loop_iter != structuredLoops.end() ; ++loop_iter){
		if(loop_iter->second.loopType == PRE_TESTED && loop_iter->second.headerNodeId == nodeId)
			return true;
	}
	return false;
}

bool ConditionalStructurer::isLatchingNodeOfPosttestedLoop(int nodeId, const std::map<int, Loop>& structuredLoops){
	for(std::map<int, Loop>::const_iterator loop_iter = structuredLoops.begin() ; loop_iter != structuredLoops.end() ; ++loop_iter){
		if(loop_iter->second.loopType == POST_TESTED && loop_iter->second.latchingNodeId == nodeId)
			return true;
	}
	return false;
}
