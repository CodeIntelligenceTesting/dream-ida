////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DerivedSequenceItem.h"

DerivedSequenceItem::DerivedSequenceItem(void){}

DerivedSequenceItem::DerivedSequenceItem(Graph _graph, VertexMap _intervals, VertexMap _originalNodesMap)
{
	graph = _graph;
	intervals = _intervals;
	originalNodesMap = _originalNodesMap;
}


DerivedSequenceItem::~DerivedSequenceItem(void)
{
}
