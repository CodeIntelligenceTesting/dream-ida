/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../GraphTypes.h"

class DerivedGraphSequenceConstructor
{
public:
	DerivedGraphSequenceConstructor(void);
	virtual ~DerivedGraphSequenceConstructor(void);
	void constructDerivedGraphSequence(Graph &cfg, DerivedGraphSequence &derivedGraphSequence);

private:
	void initializeOriginalNodesMap(const Graph &cfg, VertexMap &originalNodesMap);

	void constructIntervals(const Graph &cfg, VertexMap &intervals_out);
	bool addNodesToInterval(const Graph &cfg, std::set<Vertex> &intervalNodes);
	bool allImmediatePredecessorsInInterval(const Graph &cfg, Vertex v, std::set<Vertex> &intervalNodes);
	void getNewHeaders(const Graph &cfg, std::set<Vertex> &intervalNodes, std::set<Vertex> &currentHeaders, std::set<Vertex> &newHeaders_out);

	bool isLimitGraph(VertexMap &intervals);
	void getNextDerivedGraph(DerivedSequenceItem& previousSequenceItem, Graph &nextGraph, VertexMap &nextOriginalNodesMap);
	Vertex getCorrespondingIntervalHeader(Vertex v, VertexMap &intervals);
	bool doesEdgeExist(Vertex start, Vertex end, const Graph &g);
};

