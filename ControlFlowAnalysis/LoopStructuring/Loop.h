/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "../GraphTypes.h"
#include "LoopTypes.h"

struct Loop
{
	int headerNodeId;
	int latchingNodeId;
	int followNodeId;
	LoopType loopType;
	intSetPtr loopNodes;
	intSetPtr breakNodes;
	goToMapPtr gotoNodeMap;
};

