////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "AddressExpression.h"
#include "../../../TypeAnalysis/Types/SimpleType/PointerType.h"


AddressExpression::AddressExpression(ExpressionPtr _operand) : UnaryExpression(ADDRESS_EXPRESSION, _operand)
{
}


AddressExpression::~AddressExpression(void)
{
}


ExpressionPtr AddressExpression::deepcopy(){
	AddressExpressionPtr copyPtr = std::make_shared<AddressExpression>(operand->deepcopy());
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}


//Expression* AddressExpression::simplifyExpression(){
//	return NULL;
//}

std::string AddressExpression::getOperationString(){
	return "&";
}

TypePtr AddressExpression::getExpresstionType(TypeMapPtr typeMap){
	PointerTypePtr pointerType = std::make_shared<PointerType>();
	pointerType->pointedToType = operand->getExpresstionType(typeMap);
	return pointerType;
}

void AddressExpression::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
	if(type->type == POINTER_TYPE){
		PointerTypePtr pointerType = std::dynamic_pointer_cast<PointerType>(type);
		if(pointerType->pointedToType->type != TOP_TYPE)
			operand->updateExpressionType(pointerType->pointedToType, typeMap);
	}
}

std::string AddressExpression::to_json(){
	return "{\"expression_type\":\"AddressExpression\", \"operand\":" + operand->to_json() + "}";
}
