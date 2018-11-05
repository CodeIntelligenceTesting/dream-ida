/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/depth_first_search.hpp>

//#include "../GraphTypes.h"
#include "IntermediateRepresentation/IRSharedPointers.h"

class DFSOrderVisitor :
	public boost::default_dfs_visitor
{
public:
	DFSOrderVisitor(void);
	virtual ~DFSOrderVisitor(void);

	template < typename Vertex_t, typename Graph_t >
	void discover_vertex(Vertex_t u, const Graph_t & g) const{
		typedef typename boost::property_map<Graph_t, boost::vertex_index_t>::type IndexMap;
		IndexMap index = boost::get(boost::vertex_index,g);
		preorder->push_back(index[u]);
	};

	template < typename Vertex_t, typename Graph_t >
	void finish_vertex(Vertex_t u, const Graph_t & g) const{
		typedef typename boost::property_map<Graph_t, boost::vertex_index_t>::type IndexMap;
		IndexMap index = boost::get(boost::vertex_index,g);
		postorder->push_back(index[u]);
	};

	intVectorPtr preorder;
	intVectorPtr postorder;
};

