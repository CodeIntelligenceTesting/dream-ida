////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TopType.h"


TopType::TopType(void) : TypeBase(TOP_TYPE, "")
{
}


TopType::~TopType(void)
{
}


std::string TopType::getTypeCOLSTR(){
	return (isConst ? "const " : "") + std::string("void ");
}

bool TopType::equals(TypePtr comparedType){
	return comparedType->type == TOP_TYPE;
}

TypePtr TopType::deepcopy(){
	return std::make_shared<TopType>();
}
