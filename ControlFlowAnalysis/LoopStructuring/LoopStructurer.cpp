////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "LoopStructurer.h"

#include "../DerivedGraphSequence/DerivedGraphSequenceConstructor.h"
#include "../GraphTraversal/VertexOrdering.h"
#include "../GraphUtilities/GraphConverter.h"

#include "Loop.h"
#include "../../IntermediateRepresentation/ControlFlowGraph.h"


LoopStructurer::LoopStructurer(ControlFlowGraphPtr _controlFlowGraph){
	controlFlowGraph = _controlFlowGraph;
}

LoopStructurer::~LoopStructurer(void){}

void LoopStructurer::structureLoops(GraphStructeringInfoPtr &graphStructeringInfo){
	GraphConverter graphConverter;
	Graph cfg;
	graphConverter.convertToBoostGraph(controlFlowGraph, cfg);

	DerivedGraphSequenceConstructor derivedGraphSequenceConstructor;
	derivedGraphSequenceConstructor.constructDerivedGraphSequence(cfg, derivedGraphSequence);

	IndexMap index = boost::get(boost::vertex_index, derivedGraphSequence[0].graph);
	for(DerivedGraphSequence::iterator level_iter = derivedGraphSequence.begin() ; level_iter != derivedGraphSequence.end() ; ++level_iter){
		int level = level_iter->first;
		for(VertexMap::iterator interval_iter = level_iter->second.intervals.begin() ; interval_iter != level_iter->second.intervals.end() ; ++interval_iter){
			Vertex header = interval_iter->first;
			Vertex latchingNode;
			if(getBackEdgeNode(header, level_iter->second, latchingNode)){
				std::pair<Vertex, Vertex> headerLatchingPair;
				VertexMap* originalNodesMap = &(derivedGraphSequence[level].originalNodesMap);
				if(getOriginalHeaderLatchingNodes(originalNodesMap->at(header), originalNodesMap->at(latchingNode), headerLatchingPair)){
					Loop loop;
					loop.headerNodeId = index[headerLatchingPair.first];
					loop.latchingNodeId = index[headerLatchingPair.second];
					loop.loopNodes = getLoopNodes(level, headerLatchingPair.first, headerLatchingPair.second, interval_iter->second);
					loop.loopType = getLoopType(headerLatchingPair.first, headerLatchingPair.second, loop.loopNodes);
					findFollowNodeId(loop);
					graphStructeringInfo->loops.insert(std::pair<int, Loop>(loop.headerNodeId, loop));
					findExitNodes(loop);

					/*msg("Header: %d\n", loop.headerNodeId);
					msg("Latching Node: %d\nNodes: ", loop.latchingNodeId);
					for(std::set<int>::iterator it = loop.loopNodes->begin() ; it != loop.loopNodes->end() ; ++it){
						msg("%d, ", *it);
					}
					msg("\nFollow Node: %d\n", loop.followNodeId);
					msg("Loop Type: %d\n", loop.loopType);
					msg("Break nodes: ");
					for(std::set<int>::iterator it = loop.breakNodes->begin() ; it != loop.breakNodes->end() ; ++it)
						msg("%d, ", *it);
					msg("\n------\n");*/
				}
				/*msg("Nodes: ");
				for(std::set<Vertex>::iterator it = loopNodes->begin() ; it != loopNodes->end() ; ++it){
					msg("%d, ", *it);
				}
				msg("\n");*/
			}
		}
	}
}

bool LoopStructurer::getBackEdgeNode(Vertex header, DerivedSequenceItem &derivedSequenceItem, Vertex &latchingNode){
	InEdgeIterator e, end;
	for(boost::tie(e, end) = boost::in_edges(header, derivedSequenceItem.graph) ; e != end ; ++e){
		latchingNode = boost::source(*e, derivedSequenceItem.graph);
		std::set<Vertex>* intervalNodes = &(derivedSequenceItem.intervals[header]);
		if(intervalNodes->find(latchingNode) != intervalNodes->end())
			return true;
	}
	return false;
}


intSetPtr LoopStructurer::getLoopNodes(int level, Vertex header, Vertex latchingNode, std::set<Vertex> &intervalNodes){
	intSetPtr loopNodes = std::make_shared<std::set<int>>();
	//	std::pair<Vertex, Vertex> headerLatchingPair;
	VertexMap* originalNodesMap = &(derivedGraphSequence[level].originalNodesMap);
	//	if(getOriginalHeaderLatchingNodes(originalNodesMap->at(header), originalNodesMap->at(latchingNode), headerLatchingPair)){
	intVectorPtr reversePostorder = getReversePostorder(derivedGraphSequence[0].graph);
	IndexMap index = boost::get(boost::vertex_index, derivedGraphSequence[0].graph);
	int loopMin = getReversePostorderNumber(index[header], reversePostorder);
	int loopMax = getReversePostorderNumber(index[latchingNode], reversePostorder);
	std::set<Vertex> intervalOriginalNodes;
	for(std::set<Vertex>::iterator node_iter = intervalNodes.begin() ; node_iter != intervalNodes.end() ; ++node_iter){
		intervalOriginalNodes.insert(originalNodesMap->at(*node_iter).begin(), originalNodesMap->at(*node_iter).end());
	}
	//msg("loop: header(%d) -> latching(%d)\n", headerLatchingPair.first, headerLatchingPair.second);
	loopNodes->insert(index[header]);
	loopNodes->insert(index[latchingNode]);
	for(std::set<Vertex>::iterator origNode_iter = intervalOriginalNodes.begin() ; origNode_iter != intervalOriginalNodes.end() ; ++origNode_iter){
		Vertex testedNode = *origNode_iter;
		int nodeReversePostorderNumber = getReversePostorderNumber(index[testedNode], reversePostorder);
		if(nodeReversePostorderNumber != loopMin && nodeReversePostorderNumber != loopMax){
			LoopTestedNodes testedNodes(testedNode, latchingNode, header);
			if(doesExistPathInIntervalContainingLatchingNode(derivedGraphSequence[0].graph, testedNodes, intervalOriginalNodes)){
				loopNodes->insert(index[testedNode]);
			}
		}
	}
	//	}
	return loopNodes;
}

bool LoopStructurer::getOriginalHeaderLatchingNodes(std::set<Vertex> &headerOrigNodes, std::set<Vertex> &latchingOrigNodes, std::pair<Vertex, Vertex> &headerLatchingPair){
	for(std::set<Vertex>::iterator h_iter = headerOrigNodes.begin() ; h_iter != headerOrigNodes.end() ; ++h_iter){
		for(std::set<Vertex>::iterator l_iter = latchingOrigNodes.begin() ; l_iter != latchingOrigNodes.end() ; ++l_iter){
			if(doesEdgeExist(*l_iter, *h_iter, derivedGraphSequence[0].graph)){
				headerLatchingPair = std::make_pair(*h_iter, *l_iter);
				return true;
			}
		}
	}
	return false;
}

bool LoopStructurer::doesEdgeExist(Vertex start, Vertex end, const Graph &g){
	OutEdgeIterator e, e_end;
	for(boost::tie(e, e_end) = boost::out_edges(start, g); e != e_end; ++e){
		Vertex successor = boost::target(*e, g);
		if(successor == end)
			return true;
	}
	return false;
}

//intVectorPtr LoopStructurer::getReversePostorder(const Graph &g){
//	GraphTraversal graphTraversal(g);
//	intVectorPtr postorder = graphTraversal.postorder;
//	std::reverse(postorder->begin(), postorder->end());
//	return postorder;
//}

//int LoopStructurer::getReversePostorderNumber(int nodeIndex, intVectorPtr &reversePostorder){
//	int reversePostorderNumber = 0;
//	for(std::vector<int>::iterator r_iter = reversePostorder->begin() ; r_iter != reversePostorder->end() ; ++r_iter){
//		if(nodeIndex == *r_iter)
//			return reversePostorderNumber;
//		else
//			reversePostorderNumber++;
//	}
//	return -1;
//}

bool LoopStructurer::doesExistPathInIntervalContainingLatchingNode(Graph graph, const LoopTestedNodes &loopTestedNodes, std::set<Vertex> &intervalNodes){
	removeIntervalEdgesWhoseTargetsNotInInterval(graph, intervalNodes);
	std::set<Vertex> pathVertices;
	if(getPathVertices(graph, loopTestedNodes.testedNode, loopTestedNodes.header, pathVertices)){
		return pathVertices.find(loopTestedNodes.latchingNode) != pathVertices.end();
	}
	return false;
	//std::set<Vertex> pathInnerVertices;
	//while(getPathInnerVertices(graph, loopTestedNodes.testedNode, loopTestedNodes.header, pathInnerVertices)){
	//	bool pathIncludesLatchingNode = pathInnerVertices.find(loopTestedNodes.latchingNode) != pathInnerVertices.end();
	//	removePathVerticesInSet(pathInnerVertices, intervalNodes);
	//	if(pathInnerVertices.empty() && pathIncludesLatchingNode)
	//		return true;
	//	else{
	//		for(std::set<Vertex>::iterator del_iter = pathInnerVertices.begin() ; del_iter != pathInnerVertices.end() ; ++del_iter)
	//			boost::remove_vertex(*del_iter, graph);
	//	}
	//}
	//return false;
}

void LoopStructurer::removeIntervalEdgesWhoseTargetsNotInInterval(Graph &graph, std::set<Vertex> &intervalNodes){
	typedef boost::graph_traits<Graph>::edge_descriptor EdgeDescriptor;
	std::set<EdgeDescriptor> edgesToRemove;
	for(std::set<Vertex>::iterator v_iter = intervalNodes.begin() ; v_iter != intervalNodes.end() ; ++v_iter){
		OutEdgeIterator e, end;
		for(boost::tie(e, end) = boost::out_edges(*v_iter, graph) ; e != end ; ++e){
			if(intervalNodes.find(boost::target(*e, graph)) == intervalNodes.end())
				edgesToRemove.insert(*e);
		}
	}

	for(std::set<EdgeDescriptor>::iterator edge_iter = edgesToRemove.begin() ; edge_iter != edgesToRemove.end() ; ++edge_iter){
		boost::remove_edge(*edge_iter, graph);
	}
}

bool LoopStructurer::getPathVertices(const Graph &graph, Vertex start, Vertex end, std::set<Vertex> &pathVertices){
	//pathInnerVertices.clear();
	int numOfVertices = boost::num_vertices(graph);
	std::vector<Vertex> p(numOfVertices);
	std::vector<int> d(numOfVertices);
	boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&p[0]).distance_map(&d[0]));
	if(d[end] != (std::numeric_limits<int>::max)()){
		Vertex target = end;
		while(target != start){
			pathVertices.insert(target);
			target = p[target];
		}
		pathVertices.insert(target);
		return true;
	}
	return false;
}


LoopType LoopStructurer::getLoopType(Vertex header, Vertex latchingNode, const intSetPtr &loopNodes){
	IndexMap index = boost::get(boost::vertex_index, derivedGraphSequence[0].graph);
	if(header == latchingNode){
		return getOneNodeLoopType(header, loopNodes);
	}
	NodePtr headerBasicBlock = controlFlowGraph->nodes->at(index[header]), latchingBasicBlock = controlFlowGraph->nodes->at(index[latchingNode]);
	if(latchingBasicBlock->nodeType == TWO_WAY_NODE){
		if(headerBasicBlock->nodeType == TWO_WAY_NODE){
			return hasSuccessorNotInLoopNodes(/*header*/latchingNode, loopNodes, derivedGraphSequence[0].graph) ? POST_TESTED : PRE_TESTED/*PRE_TESTED : POST_TESTED*/;
		}
		return POST_TESTED/*PRE_TESTED*/;
	}
	else{
		if(headerBasicBlock->nodeType == TWO_WAY_NODE){
			return hasSuccessorNotInLoopNodes(header, loopNodes, derivedGraphSequence[0].graph) ? PRE_TESTED : ENDLESS;
		}
		return ENDLESS;
	}
}

LoopType LoopStructurer::getOneNodeLoopType(Vertex loopNode, const intSetPtr &loopNodes){
	return hasSuccessorNotInLoopNodes(loopNode, loopNodes, derivedGraphSequence[0].graph) ? POST_TESTED : ENDLESS;
}

bool LoopStructurer::hasSuccessorNotInLoopNodes(Vertex testedNode, const intSetPtr &loopNodes, const Graph &graph){
	IndexMap index = boost::get(boost::vertex_index, graph);
	OutEdgeIterator e, end;
	for(boost::tie(e, end) = boost::out_edges(testedNode, graph) ; e != end ; ++e){
		int successorId = index[boost::target(*e, graph)];
		if(loopNodes->find(successorId) == loopNodes->end())
			return true;
	}
	return false;
}


void LoopStructurer::findFollowNodeId(Loop &loop_out){
	if(loop_out.loopType == PRE_TESTED){
		findFollowNodeOfPreTestedLoop(loop_out);
	}
	else if(loop_out.loopType == POST_TESTED){
		findFollowNodeOfPostTestedLoop(loop_out);
	}
	else{
		findFollowNodeOfEndlessLoop(loop_out);
	}
}

void LoopStructurer::findFollowNodeOfPreTestedLoop(Loop &loop_out){
	loop_out.followNodeId = findSuccessorNotInSet(loop_out.headerNodeId, loop_out.loopNodes);
}

void LoopStructurer::findFollowNodeOfPostTestedLoop(Loop &loop_out){
	loop_out.followNodeId = findSuccessorNotInSet(loop_out.latchingNodeId, loop_out.loopNodes);
}

void LoopStructurer::findFollowNodeOfEndlessLoop(Loop &loop_out){
	const Graph &graph = derivedGraphSequence[0].graph;
	IndexMap index = boost::get(boost::vertex_index, graph);
	int followNodeId = -1, followNodePostorderNumber = boost::num_vertices(graph);
	intVectorPtr reversePostorder = getReversePostorder(graph);
	
	for(std::set<int>::iterator node_id = loop_out.loopNodes->begin() ; node_id != loop_out.loopNodes->end() ; ++node_id){
		Vertex node = boost::vertex(*node_id, graph);
		if(boost::out_degree(node, graph) >= 2){
			OutEdgeIterator e, end;
			for(boost::tie(e, end) = boost::out_edges(node, graph) ; e != end ; ++e){
				int successorId = index[boost::target(*e, graph)];
				int successorReverseOrderNumber = getReversePostorderNumber(successorId, reversePostorder);
				if(loop_out.loopNodes->find(successorId) == loop_out.loopNodes->end() && successorReverseOrderNumber < followNodePostorderNumber){
					followNodeId = successorId;
					followNodePostorderNumber = successorReverseOrderNumber;
				}
			}
		}
	}

	loop_out.followNodeId = followNodeId;
}

int LoopStructurer::findSuccessorNotInSet(int sourceNodeId, const intSetPtr &loopNodes){
	const Graph &graph = derivedGraphSequence[0].graph;
	IndexMap index = boost::get(boost::vertex_index, graph);
	Vertex source = boost::vertex(sourceNodeId, graph);
	int targetIndex = -1;
	OutEdgeIterator e, end;
	for(boost::tie(e, end) = boost::out_edges(source, graph) ; e != end ; ++e){
		targetIndex = index[boost::target(*e, graph)];
		if(loopNodes->find(targetIndex) == loopNodes->end())
			return targetIndex;
	}
	return targetIndex;
}

void LoopStructurer::findExitNodes(Loop &loop_out){
	loop_out.breakNodes = std::make_shared<std::set<int>>();
	loop_out.gotoNodeMap = std::make_shared<std::map<int, std::set<int>>>();
	Graph &graph = derivedGraphSequence[0].graph;
	IndexMap index = boost::get(boost::vertex_index, graph);
	for(std::set<int>::iterator node_iter = loop_out.loopNodes->begin() ; node_iter != loop_out.loopNodes->end() ; ++node_iter){
		int nodeId = *node_iter;
		if(haveToCheckNode(nodeId, loop_out)){
			Vertex node = boost::vertex(nodeId, graph);
			OutEdgeIterator e, end;
			for(boost::tie(e, end) = boost::out_edges(node, graph); e != end ; ++e){
				int succId = index[boost::target(*e, graph)];
				if(loop_out.loopNodes->find(succId) == loop_out.loopNodes->end()){
					if(succId == loop_out.followNodeId)
						loop_out.breakNodes->insert(succId);
					else
						loop_out.gotoNodeMap->operator[](nodeId).insert(succId);
				}
			}
		}
	}
}

bool LoopStructurer::haveToCheckNode(int nodeId, const Loop &containingLoop){
	NodeType nodeType = controlFlowGraph->nodes->at(nodeId)->nodeType;
	return (nodeType == TWO_WAY_NODE || nodeType == N_WAY_NODE) &&
			!((containingLoop.loopType == PRE_TESTED && containingLoop.headerNodeId == nodeId) || (containingLoop.loopType == POST_TESTED && containingLoop.latchingNodeId == nodeId));
}
