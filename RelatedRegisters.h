/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <string>
#include <set>

class RelatedRegisters
{
public:
	RelatedRegisters(void);
	virtual ~RelatedRegisters(void);
	std::map<std::string, std::set<std::string>> relatedRegistersMap;
};

