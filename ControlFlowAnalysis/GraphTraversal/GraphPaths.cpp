////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "GraphPaths.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

bool getPathVertices(Vertex start, Vertex end, std::vector<int> &pathVertices, Graph &graph){
	int numOfVertices = boost::num_vertices(graph);
	std::vector<Vertex> p(numOfVertices);
	std::vector<int> d(numOfVertices);
	boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&p[0]).distance_map(&d[0]));
	if(d[end] != (std::numeric_limits<int>::max)()){
		IndexMap index = boost::get(boost::vertex_index, graph);
		Vertex target = end;
		while(target != start){
			pathVertices.push_back(index[target]);
			target = p[target];
		}
		pathVertices.push_back(index[target]);
		return true;
	}
	return false;
}

int getCommonNodeId(const std::vector<int> &firstNodeVector, const std::vector<int> &secondNodeVector){
	for(std::vector<int>::const_reverse_iterator nodeId_iter = firstNodeVector.rbegin() ; nodeId_iter != firstNodeVector.rend() ; ++nodeId_iter){
		if(std::find(secondNodeVector.begin() , secondNodeVector.end(), *nodeId_iter) != secondNodeVector.end()){
			return *nodeId_iter;
		}
	}
	return -1;
}
