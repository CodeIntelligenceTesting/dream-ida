////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "RelatedRegisters.h"


RelatedRegisters::RelatedRegisters(void)
{
	relatedRegistersMap["eax"].insert("ax");
	relatedRegistersMap["eax"].insert("ah");
	relatedRegistersMap["eax"].insert("al");

	relatedRegistersMap["ax"].insert("eax");
	relatedRegistersMap["ax"].insert("ah");
	relatedRegistersMap["ax"].insert("al");

	relatedRegistersMap["ah"].insert("eax");
	relatedRegistersMap["ah"].insert("ax");

	relatedRegistersMap["al"].insert("eax");
	relatedRegistersMap["al"].insert("ax");
}


RelatedRegisters::~RelatedRegisters(void)
{
}
