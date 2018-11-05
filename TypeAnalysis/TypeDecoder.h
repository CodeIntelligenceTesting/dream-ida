/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <stdio.h>

#include <pro.h>
#include <typeinf.hpp>
#include <string>

#include "TypeSharedPointers.h"
#include "Types/SimpleType/IntegralTypes.h"

class TypeDecoder
{
public:
	TypeDecoder(void);
	virtual ~TypeDecoder(void);
	
	TypePtr decodeType(const tinfo_t &tif);
private:
	TypePtr getSimpleType(const qtype &typeString, const std::string &typeName = "");
	IntegralTypePtr getSignedIntegralType(IntegralTypeEnum specificType);
	IntegralTypePtr getUnsignedIntegralType(IntegralTypeEnum specificType);
	TypePtr getComplexType(const qtype &typeString, const std::string &typeName = "");
	bool is_type_int(type_t t);
	void decodeStructure(const std::string& struct_name);
};

