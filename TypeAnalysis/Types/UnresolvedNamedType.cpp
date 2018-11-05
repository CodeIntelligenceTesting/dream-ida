////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "UnresolvedNamedType.h"


UnresolvedNamedType::UnresolvedNamedType(std::string _typeString) : TypeBase(UNRESOLVED_NAMED_TYPE, _typeString)
{
}


UnresolvedNamedType::~UnresolvedNamedType(void)
{
}

bool UnresolvedNamedType::equals(TypePtr comparedType){
	if(comparedType->type == UNRESOLVED_NAMED_TYPE)
		return typeString.compare(comparedType->typeString) == 0;
	return false;
}

TypePtr UnresolvedNamedType::deepcopy(){
	return std::make_shared<UnresolvedNamedType>(typeString);
}
