////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "NEGFlagMacro.h"


NEGFlagMacro::NEGFlagMacro(ExpressionPtr _operand) : FlagMacro(NEGFLAGS)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	FlagPtr flagAF = std::make_shared<Flag>(CPU_FLAG, AF); definedFlags->insert(flagAF);
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF); definedFlags->insert(flagOF);
	FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF); definedFlags->insert(flagPF);
	FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF); definedFlags->insert(flagSF);
	FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF); definedFlags->insert(flagZF);

	operand = _operand;
}


NEGFlagMacro::~NEGFlagMacro(void)
{
	//delete operand;
}

std::string NEGFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = NEGFLAGS(" + operand->getExpressionString() + ")";
}

//std::vector<Expression*>* NEGFlagMacro::getUsedElements(){
//	return operand->getExpressionElements(true);
//}

void NEGFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(operand, true, usedElements_out);
}

InstructionPtr NEGFlagMacro::deepcopy(){
	ExpressionPtr operandCopy = !isExpressionPointerNull(operand) ? operand->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<NEGFlagMacro>(operandCopy);
}
