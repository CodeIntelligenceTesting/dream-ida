////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "FCOMFlagMacro.h"


FCOMFlagMacro::FCOMFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, bool _setEFLAGS) : FlagMacro(FCOMFLAGS)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	setEFLAGS = _setEFLAGS;
	if(setEFLAGS){
		//definedFlags->insert(new Flag(CPU_FLAG, ZF));
		FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF); definedFlags->insert(flagZF);
		//definedFlags->insert(new Flag(CPU_FLAG, PF));
		FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF); definedFlags->insert(flagPF);
		//definedFlags->insert(new Flag(CPU_FLAG, CF));
		FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	}
	else{
		//definedFlags->insert(new Flag(FPU_FLAG, C0));
		FlagPtr flagC0 = std::make_shared<Flag>(FPU_FLAG, C0); definedFlags->insert(flagC0);
		//definedFlags->insert(new Flag(FPU_FLAG, C2));
		FlagPtr flagC2 = std::make_shared<Flag>(FPU_FLAG, C2); definedFlags->insert(flagC2);
		//definedFlags->insert(new Flag(FPU_FLAG, C3));
		FlagPtr flagC3 = std::make_shared<Flag>(FPU_FLAG, C3); definedFlags->insert(flagC3);
	}

	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
}

FCOMFlagMacro::~FCOMFlagMacro(void)
{
	//delete firstOperand;
	//delete secondOperand;
}

std::string FCOMFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = FCOMFLAGS(" +
		( !isExpressionPointerNull(firstOperand) ? firstOperand->getExpressionString() : "NULL") + ", " + 
		( !isExpressionPointerNull(secondOperand) ? secondOperand->getExpressionString() : "NULL") + ")";
}

//std::vector<Expression*>* FCOMFlagMacro::getUsedElements(){
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
//	return usedElements;
//}

void FCOMFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(firstOperand, true, usedElements_out);
	createExpressionElements(secondOperand, true, usedElements_out);
}

InstructionPtr FCOMFlagMacro::deepcopy(){
	ExpressionPtr firstOperandCopy = !isExpressionPointerNull(firstOperand) ? firstOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr secondOperandCopy = !isExpressionPointerNull(secondOperand) ? secondOperand->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<FCOMFlagMacro>(firstOperandCopy, secondOperandCopy, setEFLAGS);
}
