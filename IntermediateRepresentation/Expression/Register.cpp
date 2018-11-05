////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <idp.hpp>
#include "Register.h"


Register::Register(uint16 _regNo, char _dtyp) : Expression(REGISTER, NO_SUBSCRIPT){
	regNo = _regNo;
	dtyp = _dtyp;
	isSimpleExpression = true;
	registerType = CPU_REGISTER;
	switch(regNo){
	case R_ax:
		if(dtyp == dt_word){
			name = "ax";
			relatedRegistersNames.insert("eax");
		}
		else if(dtyp == dt_dword){
			name = "eax";
			relatedRegistersNames.insert("ax");
		}
		relatedRegistersNames.insert("ah");
		relatedRegistersNames.insert("al");
		break;
	case R_ah:
		name = "ah";
		relatedRegistersNames.insert("eax");
		relatedRegistersNames.insert("ax");
		break;
	case R_al:
		name = "al";
		relatedRegistersNames.insert("eax");
		relatedRegistersNames.insert("ax");
		break;
	case R_cx:
		if(dtyp == dt_word){
			name = "cx";
			relatedRegistersNames.insert("ecx");
		}
		else if(dtyp == dt_dword){
			name = "ecx";
			relatedRegistersNames.insert("cx");
		}
		relatedRegistersNames.insert("ch");
		relatedRegistersNames.insert("cl");
		break;
	case R_ch:
		name = "ch";
		relatedRegistersNames.insert("ecx");
		relatedRegistersNames.insert("cx");
		break;
	case R_cl:
		name = "cl";
		relatedRegistersNames.insert("ecx");
		relatedRegistersNames.insert("cx");
		break;
	case R_dx:
		if(dtyp == dt_word){
			name = "dx";
			relatedRegistersNames.insert("edx");
		}
		else if(dtyp == dt_dword){
			name = "edx";
			relatedRegistersNames.insert("dx");
		}
		relatedRegistersNames.insert("dh");
		relatedRegistersNames.insert("dl");
		break;
	case R_dh:
		name = "dh";
		relatedRegistersNames.insert("edx");
		relatedRegistersNames.insert("dx");
		break;
	case R_dl:
		name = "dl";
		relatedRegistersNames.insert("edx");
		relatedRegistersNames.insert("dx");
		break;
	case R_bx:
		if(dtyp == dt_word){
			name = "bx";
			relatedRegistersNames.insert("ebx");
		}
		else if(dtyp == dt_dword){
			name = "ebx";
			relatedRegistersNames.insert("bx");
		}
		relatedRegistersNames.insert("bh");
		relatedRegistersNames.insert("bl");
		break;
	case R_bh:
		name = "bh";
		relatedRegistersNames.insert("ebx");
		relatedRegistersNames.insert("bx");
		break;
	case R_bl:
		name = "bl";
		relatedRegistersNames.insert("ebx");
		relatedRegistersNames.insert("bx");
		break;
	case R_sp:
		if(dtyp == dt_word){
			name = "sp";
			relatedRegistersNames.insert("esp");
		}
		else if(dtyp == dt_dword){
			name = "esp";
			relatedRegistersNames.insert("sp");
		}
		break;
	case R_bp:
		if(dtyp == dt_word){
			name = "bp";
			relatedRegistersNames.insert("ebp");
		}
		else if(dtyp == dt_dword){
			name = "ebp";
			relatedRegistersNames.insert("bp");
		}
		break;
	case R_si:
		if(dtyp == dt_word){
			name = "si";
			relatedRegistersNames.insert("esi");
		}
		else if(dtyp == dt_dword){
			name = "esi";
			relatedRegistersNames.insert("si");
		}
		break;
	case R_di:
		if(dtyp == dt_word){
			name = "di";
			relatedRegistersNames.insert("edi");
		}
		else if(dtyp == dt_dword){
			name = "edi";
			relatedRegistersNames.insert("di");
		}
		break;
	case R_st0:
		name = "st0";
		registerType = FPU_REGISTER;
		break;
	case R_st1:
		name = "st1";
		registerType = FPU_REGISTER;
		break;
	case R_st2:
		name = "st2";
		registerType = FPU_REGISTER;
		break;
	case R_st3:
		name = "st3";
		registerType = FPU_REGISTER;
		break;
	case R_st4:
		name = "st4";
		registerType = FPU_REGISTER;
		break;
	case R_st5:
		name = "st5";
		registerType = FPU_REGISTER;
		break;
	case R_st6:
		name = "st6";
		registerType = FPU_REGISTER;
		break;
	case R_st7:
		name = "st7";
		registerType = FPU_REGISTER;
		break;
	default:
		msg("Unknown register with regNo: %d" + regNo);
		name = "ERROR";
	}
	//subscript = -1;
}


Register::~Register(void)
{
}


std::string Register::getExpressionString(){
	std::string subscriptString;
	if(subscript != -1){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
		/*if(registerType == CPU_REGISTER && dtyp != dt_dword){
			subscriptString += "_CAST";
		}*/
	}
	return name + subscriptString;// + "(reg):" + expressionTypeToString();
}


char* Register::getColoredExpressionString(){
	return getColoredString(getExpressionString(), VARIABLE_COLOR);
}

std::string Register::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), VARIABLE_COLOR);
}


std::string Register::getName(){
	return name;
}

std::string Register::to_json(){
	//return "{\"expression_type\":\"Register\", \"name\":\"" + name + "\"}";
	return "{\"expression_type\":\"LocalVariable\", \"name\":\"" + getExpressionString() + "\", \"type\":\"" 
		+ expressionTypeToString() + "\", \"size_in_bytes\":\"" + boost::lexical_cast<std::string>(size_in_bytes) + "\"}";
}


//std::vector<Expression*>* Register::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->push_back(this);
//	return expElements;
//}


ExpressionPtr Register::deepcopy(){
	RegisterPtr copyPtr = std::make_shared<Register>(regNo, dtyp);
	copyPtr->subscript = subscript;
	copyPtr->phiSourceBlockId = phiSourceBlockId;
	copyPtr->expressionType = expressionType->deepcopy();
	copyPtr->set_expression_size(size_in_bytes);
	return copyPtr;
}
