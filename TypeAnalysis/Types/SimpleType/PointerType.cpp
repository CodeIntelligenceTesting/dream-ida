////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "PointerType.h"
#include "../TopType.h"

PointerType::PointerType(std::string _typeString) : SimpleType(POINTER_TYPE, _typeString)
{
	pointedToType = std::make_shared<TopType>();
}


PointerType::~PointerType(void)
{
}

std::string PointerType::getTypeCOLSTR(){
	std::string result;
	if(!typeString.empty())
		return typeString + " ";
	else{
		if(isConst)
			result = "const ";
		std::string pointedToTypeString = pointedToType->getTypeCOLSTR();
		result += pointedToTypeString.substr(0, pointedToTypeString.length() - 1) + " * ";
	}
	return result;
}

bool PointerType::equals(TypePtr comparedType){
	if(comparedType->type == POINTER_TYPE){
		PointerTypePtr comparedPointerType = std::dynamic_pointer_cast<PointerType>(comparedType);
		return pointedToType->equals(comparedPointerType->pointedToType);
	}
	return false;
}

TypePtr PointerType::deepcopy(){
	PointerTypePtr typeCopy = std::make_shared<PointerType>();
	typeCopy->pointedToType = pointedToType->deepcopy();
	return typeCopy;
}
