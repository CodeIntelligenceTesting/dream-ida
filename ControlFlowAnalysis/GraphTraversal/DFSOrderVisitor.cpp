////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DFSOrderVisitor.h"

DFSOrderVisitor::DFSOrderVisitor(void)
{
	preorder = std::make_shared<std::vector<int>>();
	postorder = std::make_shared<std::vector<int>>();
}


DFSOrderVisitor::~DFSOrderVisitor(void)
{
}

//template < typename Vertex, typename Graph >
//void DFSOrderVisitor::discover_vertex(Vertex u, const Graph & g) const{
//	typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//	IndexMap index = boost::get(boost::vertex_index,g);
//	preorder->push_back(index[u]);
//}
//
//template < typename Vertex, typename Graph >
//void DFSOrderVisitor::finish_vertex(Vertex u, const Graph & g) const{
//	typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//	IndexMap index = boost::get(boost::vertex_index,g);
//	postorder->push_back(index[u]);
//}
