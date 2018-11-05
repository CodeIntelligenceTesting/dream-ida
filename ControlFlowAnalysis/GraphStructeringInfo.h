/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "LoopStructuring/Loop.h"
#include "ConditionalsStructuring/Conditional.h"
#include "NWayStructuring/NWaySwitch.h"

struct GraphStructeringInfo
{
	std::map<int, Loop> loops;
	std::map<int, Conditional> conditionals;
	std::map<int, NWaySwitch> switches;
};

