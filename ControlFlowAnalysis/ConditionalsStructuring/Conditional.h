/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "ConditionalTypes.h"

struct Conditional
{
	Conditional(){};
	Conditional(int _headerNodeId, int _followNodeId, ConditionalType _conditionalType){
		headerNodeId = _headerNodeId;
		followNodeId = _followNodeId;
		conditionalType = _conditionalType;
	};
	int headerNodeId;
	int followNodeId;
	ConditionalType conditionalType;
};

