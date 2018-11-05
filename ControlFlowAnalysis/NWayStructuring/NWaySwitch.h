/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"

class NWaySwitch
{
public:
	NWaySwitch(){
		headerNodeId = -1;
		followNodeId = -1;
	};
	NWaySwitch(int _headerNodeId, int _followNodeId){
		headerNodeId = _headerNodeId;
		followNodeId = _followNodeId;
	};
	virtual ~NWaySwitch(void);
	void structureNWay();

	int headerNodeId;
	int followNodeId;
};

