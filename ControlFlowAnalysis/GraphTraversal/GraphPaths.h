/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../GraphTypes.h"

bool getPathVertices(Vertex start, Vertex end, std::vector<int> &pathVertices, Graph &graph);
int getCommonNodeId(const std::vector<int> &firstNodeVector, const std::vector<int> &secondNodeVector);
