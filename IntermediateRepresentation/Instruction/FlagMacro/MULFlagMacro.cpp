////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "MULFlagMacro.h"


MULFlagMacro::MULFlagMacro(ExpressionPtr _result) : FlagMacro(MULFLAGS)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF); definedFlags->insert(flagOF);

	result = _result;
}


MULFlagMacro::~MULFlagMacro(void)
{
	//delete result;
}

std::string MULFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = MULFLAGS(" + result->getExpressionString() + ")";
}

//std::vector<Expression*>* MULFlagMacro::getUsedElements(){
//	return result->getExpressionElements(true);
//}

void MULFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(result, true, usedElements_out);
}

InstructionPtr MULFlagMacro::deepcopy(){
	ExpressionPtr resultCopy = !isExpressionPointerNull(result) ? result->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<MULFlagMacro>(resultCopy);
}
