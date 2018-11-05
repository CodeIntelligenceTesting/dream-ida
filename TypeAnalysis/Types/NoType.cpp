////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "NoType.h"


NoType::NoType(void) : TypeBase(NO_TYPE, "")
{
}


NoType::~NoType(void)
{
}

bool NoType::equals(TypePtr comparedType){
	return comparedType->type == NO_TYPE;
}

TypePtr NoType::deepcopy(){
	return std::make_shared<NoType>();
}
