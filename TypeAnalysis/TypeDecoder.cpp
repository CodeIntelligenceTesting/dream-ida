////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TypeDecoder.h"

#include <struct.hpp>
#include <typeinf.hpp>

#include "TypeAnalysis/Types/TopType.h"
#include "TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "TypeAnalysis/Types/SimpleType/PointerType.h"
#include "TypeAnalysis/Types/UnresolvedNamedType.h"

#include "TypeAnalysis/Types/ComposedType/ClassType.h"
#include "TypeAnalysis/Types/ComposedType/UnionType.h"

TypeDecoder::TypeDecoder(void)
{
}


TypeDecoder::~TypeDecoder(void)
{
}

TypePtr TypeDecoder::decodeType(const tinfo_t &tif){
	return std::make_shared<TopType>();
}
/*
TypePtr TypeDecoder::decodeType(const qtype &typeString, const std::string &typeName){
	TypePtr resultType = std::make_shared<TopType>();
	if(is_type_ptr(typeString[0])){
		PointerTypePtr pointerType = std::make_shared<PointerType>();
		qtype subType = typeString.substr(1);
		pointerType->pointedToType = is_type_complex(subType[0]) ? getComplexType(subType, typeName) : getSimpleType(subType, typeName);
		resultType = pointerType;
	}
	else if(is_type_complex(typeString[0])){
		resultType = getComplexType(typeString, typeName);
	}
	else
		resultType = getSimpleType(typeString, typeName);
	return resultType;
}
*/
TypePtr TypeDecoder::getSimpleType(const qtype &typeString, const std::string &typeName){
	TypePtr resultType = std::make_shared<TopType>();
	// return resultType;
	
	// if(is_type_int64(typeString[0]))
	// 	resultType = getSignedIntegralType(LONG_LONG_TYPE);
	// if(is_type_int(typeString[0]))
	// 	resultType = getSignedIntegralType(INT_TYPE);
	// if(is_type_short(typeString[0]))
	// 	resultType = getSignedIntegralType(SHORT_TYPE);
	// if(is_type_long(typeString[0]))
	// 	resultType = getSignedIntegralType(INT_TYPE);
	// if(is_type_char(typeString[0]))
	// 	resultType = getSignedIntegralType(CHAR_TYPE);
	// if(is_type_ldouble(typeString[0]))
	// 	resultType = getSignedIntegralType(LONG_DOUBLE_TYPE);
	// if(is_type_double(typeString[0]))
	// 	resultType = getSignedIntegralType(DOUBLE_TYPE);
	// if(is_type_float(typeString[0]))
	// 	resultType = getSignedIntegralType(FLOAT_TYPE);
	// if(is_type_bool(typeString[0]))
	// 	resultType = getSignedIntegralType(BOOL_TYPE);
	// if(is_type_void(typeString[0]))
	// 	resultType = std::make_shared<TopType>();
	// if(is_type_uint(typeString[0]))
	// 	resultType = getUnsignedIntegralType(INT_TYPE);
	// if(is_type_uchar(typeString[0]))
	// 	resultType = getUnsignedIntegralType(CHAR_TYPE);
	// if(is_type_ushort(typeString[0]))
	// 	resultType = getUnsignedIntegralType(SHORT_TYPE);
	// if(is_type_ulong(typeString[0]))
	// 	resultType = getUnsignedIntegralType(INT_TYPE);
	// if(is_type_uint64(typeString[0]))
	// 	resultType = getUnsignedIntegralType(LONG_LONG_TYPE);
	// if(is_type_const(typeString[0]))
	// 	resultType->isConst = true;
	// resultType->typeString = typeName;
	return resultType;
}

TypePtr TypeDecoder::getComplexType(const qtype &typeString, const std::string &typeName){
	TypePtr resultType = std::make_shared<TopType>();
	// if(is_type_typedef(typeString[0])){
	// 	std::string currentTypeName((char*)(typeString.substr(2).c_str()));
	// 	type_t* p = (type_t*)typeString.c_str();
	// 	if(is_type_resolvable(p)){
	// 		const type_t* resolved = resolve_typedef2(idati, p);

	// 		resultType = decodeType(resolved, currentTypeName);
	// 		resultType->typeString = currentTypeName;
	// 	}
	// 	else{
	// 		resultType = std::make_shared<UnresolvedNamedType>(currentTypeName);
	// 	}
	// }
	// if(is_type_const(typeString[0]))
	// 	resultType->isConst = true;
	return resultType;
}

bool TypeDecoder::is_type_int(type_t t){
	return get_full_type(t) == (BT_INT|BTMT_UNKSIGN)
		   || get_full_type(t) == (BT_INT|BTMT_SIGNED); 
}

void TypeDecoder::decodeStructure(const std::string& struct_name){
	// tid_t struct_id = til2idb(-1, struct_name.c_str());
	// struc_t * st = get_struc(struct_id);
	// if(st != NULL){
	// 	for(unsigned int i = 0 ; i < st->memqty ; ++i){
	// 		member_t m = (st->members)[i];
	// 		qtype type, fields;
	// 		if(get_member_tinfo(&m, &type, &fields)){
	// 			decodeType(type);
	// 		}
	// 	}
	// }
}

IntegralTypePtr TypeDecoder::getSignedIntegralType(IntegralTypeEnum specificType){
	return std::make_shared<IntegralType>(specificType);
}

IntegralTypePtr TypeDecoder::getUnsignedIntegralType(IntegralTypeEnum specificType){
	IntegralTypePtr resultType = getSignedIntegralType(specificType);
	resultType->isSigned = false;
	return resultType;
}
