////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Flag.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralTypes.h"


Flag::Flag(FlagType _flagType, FlagNo _flagNo) : Expression(FLAG, NO_SUBSCRIPT)
{
	flagType = _flagType;
	flagNo = _flagNo;
	isSimpleExpression = true;
	expressionType = std::make_shared<IntegralType>(BOOL_TYPE);
	//subscript = -1;
	switch(flagNo){
	case CF:
		name = "CF";
		break;
	case PF:
		name = "PF";
		break;
	case AF:
		name = "AF";
		break;
	case ZF:
		name = "ZF";
		break;
	case SF:
		name = "SF";
		break;
	case DF:
		name = "DF";
		break;
	case OF:
		name = "OF";
		break;
	case C0:
		name = "C0";
		break;
	case C1:
		name = "C1";
		break;
	case C2:
		name = "C2";
		break;
	case C3:
		name = "C3";
		break;
	default:
		msg("Flag Variable Error\n");
		name = "ERROR";
	}
}


Flag::~Flag(void)
{
}


std::string Flag::getExpressionString(){
	std::string subscriptString;
	if(subscript != -1){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
	}
	return name + subscriptString;
}


char* Flag::getColoredExpressionString(){
	return getColoredString(getExpressionString(), VARIABLE_COLOR);
}

std::string Flag::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), VARIABLE_COLOR);
}

std::string Flag::getName(){
	return name;
}


ExpressionPtr Flag::deepcopy(){
	FlagPtr copyFlagPtr = std::make_shared<Flag>(flagType, flagNo);//new Flag(flagType, flagNo);
	copyFlagPtr->name = name;
	copyFlagPtr->subscript = subscript;
	copyFlagPtr->size_in_bytes = size_in_bytes;
	copyFlagPtr->expressionType = expressionType->deepcopy();
	copyFlagPtr->phiSourceBlockId = phiSourceBlockId;
	return copyFlagPtr;
}


std::string Flag::to_json(){
	return "{\"expression_type\":\"LocalVariable\", \"name\":\"" + getExpressionString() + "\", \"type\":\"bool\"}";
}

//void Flag::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->push_back(this);
//	return expElements;
//}
