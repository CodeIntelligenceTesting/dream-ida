/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "../GraphTypes.h"

class ControlFlowGraph;

class GraphConverter
{
public:
	GraphConverter(void);
	virtual ~GraphConverter(void);

	void convertToBoostGraph(ControlFlowGraphPtr controlFlowGraph, Graph &boostGraph);
};

