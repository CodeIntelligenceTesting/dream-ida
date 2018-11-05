////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "INCFlagMacro.h"


INCFlagMacro::INCFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result) : FlagMacro(INCFLAGS)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	/*definedFlags->insert(new Flag(CPU_FLAG, AF));
	definedFlags->insert(new Flag(CPU_FLAG, CF));
	definedFlags->insert(new Flag(CPU_FLAG, OF));
	definedFlags->insert(new Flag(CPU_FLAG, PF));
	definedFlags->insert(new Flag(CPU_FLAG, SF));
	definedFlags->insert(new Flag(CPU_FLAG, ZF));*/
	FlagPtr flagAF = std::make_shared<Flag>(CPU_FLAG, AF); definedFlags->insert(flagAF);
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF); definedFlags->insert(flagOF);
	FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF); definedFlags->insert(flagPF);
	FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF); definedFlags->insert(flagSF);
	FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF); definedFlags->insert(flagZF);
	
	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
	result = _result;
}


INCFlagMacro::~INCFlagMacro(void)
{
	/*delete firstOperand;
	delete secondOperand;
	delete result;*/
}

std::string INCFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = INCFLAGS(" +
		(!isExpressionPointerNull(firstOperand) ? firstOperand->getExpressionString() : "NULL") + ", " + 
		(!isExpressionPointerNull(secondOperand) ? secondOperand->getExpressionString() : "NULL") + ", " + 
		(!isExpressionPointerNull(result) ? result->getExpressionString() : "NULL") + ")";/*	
			+ firstOperand->getExpressionString() + ", " + secondOperand->getExpressionString()
			+ ", " + result->getExpressionString() + ")";*/
}

//std::vector<Expression*>* INCFlagMacro::getUsedElements(){
//	std::vector<Expression*>* usedElements = new std::vector<Expression*>();
//	if(firstOperand != NULL){
//		std::vector<Expression*>* firstOperandElements = firstOperand->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), firstOperandElements->begin(), firstOperandElements->end());
//		delete firstOperandElements;
//	}
//	if(secondOperand != NULL){
//		std::vector<Expression*>* secondOperandElements = secondOperand->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), secondOperandElements->begin(), secondOperandElements->end());
//		delete secondOperandElements;
//	}
//	if(result != NULL){
//		std::vector<Expression*>* resultElements = result->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), resultElements->begin(), resultElements->end());
//		delete resultElements;
//	}
//	return usedElements;
//}

void INCFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(firstOperand, true, usedElements_out);
	createExpressionElements(secondOperand, true, usedElements_out);
	createExpressionElements(result, true, usedElements_out);
}

InstructionPtr INCFlagMacro::deepcopy(){
	ExpressionPtr firstOperandCopy = !isExpressionPointerNull(firstOperand) ? firstOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr secondOperandCopy = !isExpressionPointerNull(secondOperand) ? secondOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr resultCopy = !isExpressionPointerNull(result) ? result->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<INCFlagMacro>(firstOperandCopy, secondOperandCopy, resultCopy);
}
