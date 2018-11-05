////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DerivedGraphSequenceConstructor.h"
#include "DerivedSequenceItem.h"

/////Delete/////
#include <ida.hpp>
#include <idp.hpp>
#include <graph.hpp>
#include <kernwin.hpp>
#include <loader.hpp>
//////////


DerivedGraphSequenceConstructor::DerivedGraphSequenceConstructor(void)
{
}


DerivedGraphSequenceConstructor::~DerivedGraphSequenceConstructor(void)
{
}

void DerivedGraphSequenceConstructor::constructDerivedGraphSequence(Graph &cfg, DerivedGraphSequence &derivedGraphSequence){
	VertexMap intervals, originalNodesMap;

	initializeOriginalNodesMap(cfg, originalNodesMap);
	constructIntervals(cfg, intervals);
	derivedGraphSequence[0] = DerivedSequenceItem(cfg, intervals, originalNodesMap);

	msg("\nIntervals:\n");
	for(std::map<Vertex, std::set<Vertex>>::iterator it = intervals.begin() ; it != intervals.end() ; ++it){
		msg("\t Header: %d\n", it->first);
		msg("\t Nodes: ");
		for(std::set<Vertex>::iterator i = it->second.begin() ; i != it->second.end() ; ++i){
			msg("%d, ", *i);
		}
		msg("\n");
	}

	int level = 0;
	while(!isLimitGraph(derivedGraphSequence[level].intervals)){
		Graph nextGraph;
		intervals.clear();
		originalNodesMap.clear();
		getNextDerivedGraph(derivedGraphSequence[level], nextGraph, originalNodesMap);
		constructIntervals(nextGraph, intervals);
		level++;
		derivedGraphSequence[level] = DerivedSequenceItem(nextGraph, intervals, originalNodesMap);
	}

	/*for(DerivedGraphSequence::iterator dgit = derivedGraphSequence.begin() ; dgit != derivedGraphSequence.end() ; ++dgit){
		msg("level(%d)\n", dgit->first);
		for(VertexMap::iterator it = dgit->second.intervals.begin() ; it != dgit->second.intervals.end() ; ++it){
			msg("interval(%d) = {", it->first);
			for(std::set<Vertex>::iterator sit = it->second.begin() ; sit != it->second.end() ; ++sit){
				msg("%d, ", *sit);
			}
			msg("}\n");
		}
		for(VertexMap::iterator it = dgit->second.originalNodesMap.begin() ; it != dgit->second.originalNodesMap.end() ; ++it){
			msg("originalNodes(%d) = {", it->first);
			for(std::set<Vertex>::iterator sit = it->second.begin() ; sit != it->second.end() ; ++sit){
				msg("%d, ", *sit);
			}
			msg("}\n");
		}
		Graph g = dgit->second.graph;
		boost::graph_traits<Graph>::edge_iterator it, end;
		for(boost::tie(it, end) = boost::edges(g) ; it != end ; ++it){
			msg("%d -> %d\n", boost::source(*it, g), boost::target(*it, g));
		}
		msg("-----\n");
	}*/
}

void DerivedGraphSequenceConstructor::initializeOriginalNodesMap(const Graph &cfg, VertexMap &originalNodesMap){
	VertexIterator i, end;
	for(boost::tie(i, end) = boost::vertices(cfg) ; i != end ; ++i){
		Vertex v = *i;
		originalNodesMap[v].insert(v);
	}
}

void DerivedGraphSequenceConstructor::constructIntervals(const Graph &cfg, VertexMap &intervals_out){
	std::set<Vertex> headers, newHeaders, processedHeaders;
	newHeaders.insert(boost::vertex(0, cfg));
	while(!newHeaders.empty()){
		headers.insert(newHeaders.begin(), newHeaders.end());
		newHeaders.clear();
		for(std::set<Vertex>::iterator header_iter = headers.begin() ; header_iter != headers.end() ; ++header_iter){
			Vertex header = *header_iter;
			if(processedHeaders.find(header) == processedHeaders.end()){
				processedHeaders.insert(header);
				std::set<Vertex> intervalNodes;
				intervalNodes.insert(header);
				while(addNodesToInterval(cfg, intervalNodes));
				intervals_out[header] = intervalNodes;
				getNewHeaders(cfg, intervalNodes, headers, newHeaders);
			}
		}
	}
}

bool DerivedGraphSequenceConstructor::addNodesToInterval(const Graph &cfg, std::set<Vertex> &intervalNodes){
	std::set<Vertex> nodesToAdd;
	for(std::set<Vertex>::iterator node_iter = intervalNodes.begin() ; node_iter != intervalNodes.end() ; ++node_iter){
		Vertex node = *node_iter;
		OutEdgeIterator e, e_end;
		for(boost::tie(e, e_end) = boost::out_edges(node, cfg); e != e_end; ++e){
			Vertex successor = boost::target(*e, cfg);
			if(intervalNodes.find(successor) == intervalNodes.end() && allImmediatePredecessorsInInterval(cfg, successor, intervalNodes))
				nodesToAdd.insert(successor);
		}
	}
	if(nodesToAdd.empty())
		return false;
	else{
		intervalNodes.insert(nodesToAdd.begin(), nodesToAdd.end());
		return true;
	}
}

bool DerivedGraphSequenceConstructor::allImmediatePredecessorsInInterval(const Graph &cfg, Vertex v, std::set<Vertex> &intervalNodes){
	InEdgeIterator e, e_end;
	for(boost::tie(e, e_end) = boost::in_edges(v, cfg); e != e_end; ++e)
		if(intervalNodes.find(boost::source(*e, cfg)) == intervalNodes.end())
			return false;
	return true;
}

void DerivedGraphSequenceConstructor::getNewHeaders(const Graph &cfg, std::set<Vertex> &intervalNodes, std::set<Vertex> &currentHeaders, std::set<Vertex> &newHeaders_out){
	for(std::set<Vertex>::iterator node_iter = intervalNodes.begin() ; node_iter != intervalNodes.end() ; ++node_iter){
		Vertex node = *node_iter;
		OutEdgeIterator e, e_end;
		for(boost::tie(e, e_end) = boost::out_edges(node, cfg); e != e_end; ++e){
			Vertex successor = boost::target(*e, cfg);
			if(intervalNodes.find(successor) == intervalNodes.end() && currentHeaders.find(successor) == currentHeaders.end())
				newHeaders_out.insert(successor);
		}
	}
}


bool DerivedGraphSequenceConstructor::isLimitGraph(VertexMap &intervals){
	for(VertexMap::iterator it = intervals.begin() ; it != intervals.end() ; ++it)
		if(it->second.size() > 1)
			return false;
	return true;
}

void DerivedGraphSequenceConstructor::getNextDerivedGraph(DerivedSequenceItem& previousSequenceItem, Graph &nextGraph, VertexMap &nextOriginalNodesMap){
	std::map<Vertex, Vertex> intervalHeadToVertexMap;
	for(VertexMap::iterator interval_iter = previousSequenceItem.intervals.begin() ; interval_iter != previousSequenceItem.intervals.end() ; ++interval_iter){
		Vertex v = boost::add_vertex(nextGraph);
		intervalHeadToVertexMap[interval_iter->first] = v;
		for(std::set<Vertex>::iterator intervalNode_iter = interval_iter->second.begin() ; intervalNode_iter != interval_iter->second.end() ; ++intervalNode_iter){
			Vertex previousIntervalNode = *intervalNode_iter;
			std::set<Vertex>* previousOriginalNodes = &(previousSequenceItem.originalNodesMap[previousIntervalNode]);
			nextOriginalNodesMap[v].insert(previousOriginalNodes->begin(), previousOriginalNodes->end());
		}
	}
	EdgeWeightProperty edgeWeight = 1;
	for(VertexMap::iterator interval_iter = previousSequenceItem.intervals.begin() ; interval_iter != previousSequenceItem.intervals.end() ; ++interval_iter){
		Vertex header = interval_iter->first;
		
		InEdgeIterator pred, in_end;
		for(boost::tie(pred, in_end) = boost::in_edges(header, previousSequenceItem.graph) ; pred != in_end ; ++pred){
			Vertex predecessorNode = boost::source(*pred, previousSequenceItem.graph);
			if(interval_iter->second.find(predecessorNode) == interval_iter->second.end()){
				Vertex nextEdgeStart = intervalHeadToVertexMap[getCorrespondingIntervalHeader(predecessorNode, previousSequenceItem.intervals)];
				Vertex nextEdgeEnd = intervalHeadToVertexMap[header];
				if(!doesEdgeExist(nextEdgeStart, nextEdgeEnd, nextGraph))
					boost::add_edge(nextEdgeStart, nextEdgeEnd, edgeWeight, nextGraph);
			}
		}

		for(std::set<Vertex>::iterator intervalNode_iter = interval_iter->second.begin() ; intervalNode_iter != interval_iter->second.end() ; ++intervalNode_iter){
			Vertex intervalNode = *intervalNode_iter;
			OutEdgeIterator succ, out_end;
			for(boost::tie(succ, out_end) = boost::out_edges(intervalNode, previousSequenceItem.graph) ; succ != out_end ; ++succ){
				Vertex successorNode = boost::target(*succ, previousSequenceItem.graph);
				if(interval_iter->second.find(successorNode) == interval_iter->second.end()){
					Vertex nextEdgeStart = intervalHeadToVertexMap[header];
					Vertex nextEdgeEnd = intervalHeadToVertexMap[getCorrespondingIntervalHeader(successorNode, previousSequenceItem.intervals)];
					if(!doesEdgeExist(nextEdgeStart, nextEdgeEnd, nextGraph))
						boost::add_edge(nextEdgeStart, nextEdgeEnd, edgeWeight, nextGraph);
				}
			}
		}
	}
}

Vertex DerivedGraphSequenceConstructor::getCorrespondingIntervalHeader(Vertex v, VertexMap &intervals){
	for(VertexMap::iterator interval_iter = intervals.begin() ; interval_iter != intervals.end() ; ++interval_iter){
		if(interval_iter->second.find(v) != interval_iter->second.end())
			return interval_iter->first;
	}
}

bool DerivedGraphSequenceConstructor::doesEdgeExist(Vertex start, Vertex end, const Graph &g){
	OutEdgeIterator e, e_end;
	for(boost::tie(e, e_end) = boost::out_edges(start, g); e != e_end; ++e){
		Vertex successor = boost::target(*e, g);
		if(successor == end)
			return true;
	}
	return false;
}
