////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TypeBase.h"


TypeBase::TypeBase(TypeEmun _type ,std::string _typeString)
{
	type = _type;
	typeString = _typeString;
	isConst = false;
}


TypeBase::~TypeBase(void)
{
}


std::string TypeBase::getTypeFromSize(int size_in_bytes){
	switch(size_in_bytes){
	case 1:
		return "char";
	case 2:
		return "short";
	default:
		return "int";
	}
}
