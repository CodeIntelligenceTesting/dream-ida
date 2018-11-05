/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../GraphTypes.h"

class DerivedSequenceItem
{
public:
	DerivedSequenceItem(void);
	DerivedSequenceItem(Graph _graph, VertexMap _intervals, VertexMap _originalNodesMap);
	virtual ~DerivedSequenceItem(void);

	Graph graph;
	VertexMap intervals;
	VertexMap originalNodesMap;
};

