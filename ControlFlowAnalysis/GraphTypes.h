/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <set>
#include <utility>
#include <vector>
#include <memory>

#include <boost/graph/adjacency_list.hpp>

typedef std::pair<int, int> Edge;
typedef boost::property<boost::edge_weight_t, int> EdgeWeightProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, EdgeWeightProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexIndexMap;
typedef std::map<Vertex, std::set<Vertex>> VertexMap;
typedef boost::graph_traits<Graph>::in_edge_iterator InEdgeIterator;
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;
typedef std::shared_ptr<std::set<Vertex>> VertexSetPtr;
typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;

class DerivedSequenceItem;
typedef std::map<int, DerivedSequenceItem> DerivedGraphSequence;

struct GraphStructeringInfo;
typedef std::shared_ptr<GraphStructeringInfo> GraphStructeringInfoPtr;

typedef boost::iterator_property_map<std::vector<Vertex>::iterator, IndexMap> PredMap;
