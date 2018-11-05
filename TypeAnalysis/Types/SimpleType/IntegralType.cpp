////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "IntegralType.h"


IntegralType::IntegralType(IntegralTypeEnum _integralType, std::string _typeString) : SimpleType(INTEGRAL_TYPE, _typeString)
{
	size = -1;
	isSigned = false;
	integralType = _integralType;
}


IntegralType::~IntegralType(void)
{
}

std::string IntegralType::getTypeCOLSTR(){
	std::string space = " ";
	std::string constString = isConst ? "const " : "";
	if(!typeString.empty()){
		return constString + typeString + space;
	}
	switch(integralType){
	case SIZE8_TYPE:
		return constString + "size_8" + space;
	case CHAR_TYPE:
		return constString + "char" + space;
	case BOOL_TYPE:
		return constString + "bool" + space;
	case SIZE16_TYPE:
		return constString + "size_16" + space;
	case SHORT_TYPE:
		return constString + "short" + space;
	case SIZE32_TYPE:
		return constString + "size_32" + space;
	case FLOAT_TYPE:
		return constString + "float" + space;
	case POINTER_OR_INTEGER_TYPE:
		return constString + "pointer_or_integer" + space;
	case INT_TYPE:
		return constString + "int" + space;
	/*case POINTER_TYPE:
		return constString + "pointer";*/
	case SIZE64_TYPE:
		return constString + "size_64" + space;
	case DOUBLE_TYPE:
		return constString + "double" + space;
	case LONG_LONG_TYPE:
		return constString + "long long" + space;
	case LONG_DOUBLE_TYPE:
		return constString + "long double" + space;
	default:
		return constString + "Integral Type Error" + space;
	}
}

bool IntegralType::equals(TypePtr comparedType){
	if(comparedType->type == INTEGRAL_TYPE){
		IntegralTypePtr comparedIntegralType = std::dynamic_pointer_cast<IntegralType>(comparedType);
		return integralType == comparedIntegralType->integralType;
	}
	return false;
}

TypePtr IntegralType::deepcopy(){
	return std::make_shared<IntegralType>(integralType);
}
