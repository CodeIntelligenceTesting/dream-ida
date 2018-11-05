/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "SimpleType.h"

#include "IntegralTypes.h"

class IntegralType :
	public SimpleType
{
public:
	IntegralType(IntegralTypeEnum _integralType, std::string _typeString = "");
	virtual ~IntegralType(void);

	std::string getTypeCOLSTR();

	bool isIntegerType(){return integralType == INT_TYPE;};
	bool isFloatType(){return integralType == DOUBLE_TYPE;};
	bool equals(TypePtr comparedType);
	TypePtr deepcopy();

//private:
	int size;
	bool isSigned;
	IntegralTypeEnum integralType;
};

