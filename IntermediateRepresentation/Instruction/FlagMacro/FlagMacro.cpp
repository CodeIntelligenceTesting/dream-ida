////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "FlagMacro.h"
//#include "../../../Shared/houseKeepingUtilities.h"

FlagMacro::FlagMacro(FlagMacroType _flagMacroType) : Instruction(FLAG_MACRO)
{
	flagMacroType = _flagMacroType;
}


FlagMacro::~FlagMacro(void)
{
	//msg("deleting.. %s\n", getInstructionString().c_str());
	//deleteSetPointer(definedFlags); definedFlags = NULL;
}

char* FlagMacro::getColoredInstructionString(){
	return getColoredString(getInstructionString(), MACRO_COLOR);
}

std::string FlagMacro::getInstructionCOLSTR(){
	return getCOLSTR(getInstructionString(), MACRO_COLOR);
}

//std::vector<Expression*>* FlagMacro::getDefinedElements(){
//	std::vector<Expression*>* definedElements = new std::vector<Expression*>();
//	definedElements->insert(definedElements->end(), definedFlags->begin(), definedFlags->end());
//	return definedElements;
//}

void FlagMacro::getDefinedElements(std::vector<ExpressionPtr>& definedElements_out){
	definedElements_out.insert(definedElements_out.end(), definedFlags->begin(), definedFlags->end());
}

bool FlagMacro::replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	for(std::set<FlagPtr>::iterator it = definedFlags->begin() ; it != definedFlags->end() ; ++it){
		if((*it)->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			(*it)->name = replacingExpression->getExpressionString();
			(*it)->subscript = NO_SUBSCRIPT;
			return true;
		}
	}
	return false;
}

//FlagSetPtr FlagMacro::getDefinedFlagsCopy(){
//	FlagSetPtr definedFlagsCopy = std::make_shared<std::set<FlagPtr>>();
//	for(std::set<FlagPtr>::iterator flag_iter = definedFlags->begin() ; flag_iter != definedFlags->end() ; ++flag_iter){
//		definedFlagsCopy->insert((*flag_iter)->deepcopy());
//	}
//	return definedFlagsCopy;
//}
