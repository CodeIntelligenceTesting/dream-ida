////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "VertexOrdering.h"

#include "GraphTraversal.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>

intVectorPtr getReversePostorder(const Graph &g){
	GraphTraversal graphTraversal(g);
	intVectorPtr postorder = graphTraversal.postorder;
	std::reverse(postorder->begin(), postorder->end());
	return postorder;
}

int getReversePostorderNumber(int nodeIndex, intVectorPtr &reversePostorder){
	int reversePostorderNumber = 0;
	for(std::vector<int>::iterator r_iter = reversePostorder->begin() ; r_iter != reversePostorder->end() ; ++r_iter){
		if(nodeIndex == *r_iter)
			return reversePostorderNumber;
		else
			reversePostorderNumber++;
	}
	return -1;
}

//bool getPathVertices(const Graph &graph, Vertex start, Vertex end, std::set<Vertex> &pathVertices){
//	//pathInnerVertices.clear();
//	int numOfVertices = boost::num_vertices(graph);
//	std::vector<Vertex> p(numOfVertices);
//	std::vector<int> d(numOfVertices);
//	boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&p[0]).distance_map(&d[0]));
//	if(d[end] != (std::numeric_limits<int>::max)()){
//		Vertex target = end;
//		while(target != start){
//			pathVertices.insert(target);
//			target = p[target];
//		}
//		pathVertices.insert(target);
//		return true;
//	}
//	return false;
//}
