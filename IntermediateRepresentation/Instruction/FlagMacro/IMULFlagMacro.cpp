////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "IMULFlagMacro.h"


IMULFlagMacro::IMULFlagMacro(ExpressionPtr _result) : FlagMacro(IMULFLAGS)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	//definedFlags->insert(new Flag(CPU_FLAG, CF));
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	//definedFlags->insert(new Flag(CPU_FLAG, OF));
	FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF); definedFlags->insert(flagOF);

	result = _result;
}


IMULFlagMacro::~IMULFlagMacro(void)
{
	//delete result;
}

std::string IMULFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = IMULFLAGS(" + result->getExpressionString() + ")";
}

//std::vector<Expression*>* IMULFlagMacro::getUsedElements(){
//	return result->getExpressionElements(true);
//}

void IMULFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(result, true, usedElements_out);
}

InstructionPtr IMULFlagMacro::deepcopy(){
	ExpressionPtr resultCopy = !isExpressionPointerNull(result) ? result->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<IMULFlagMacro>(resultCopy);
}
