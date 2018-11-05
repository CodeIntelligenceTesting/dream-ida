/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../GraphTypes.h"
#include "../../IntermediateRepresentation/IRSharedPointers.h"

intVectorPtr getReversePostorder(const Graph &g);
int getReversePostorderNumber(int nodeIndex, intVectorPtr &reversePostorder);
//bool getPathVertices(const Graph &graph, Vertex start, Vertex end, std::set<Vertex> &pathVertices);
