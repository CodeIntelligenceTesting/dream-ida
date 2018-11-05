////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "FloatingPoint_SUBFlagMacro.h"


FloatingPoint_SUBFlagMacro::FloatingPoint_SUBFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result) : FlagMacro(SUBFLAGS_FLOATING_POINT)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	//definedFlags->insert(new Flag(FPU_FLAG, C0));
	FlagPtr flagC0 = std::make_shared<Flag>(FPU_FLAG, C0); definedFlags->insert(flagC0);
	//definedFlags->insert(new Flag(FPU_FLAG, C1));
	FlagPtr flagC1 = std::make_shared<Flag>(FPU_FLAG, C1); definedFlags->insert(flagC1);
	//definedFlags->insert(new Flag(FPU_FLAG, C2));
	FlagPtr flagC2 = std::make_shared<Flag>(FPU_FLAG, C2); definedFlags->insert(flagC2);
	//definedFlags->insert(new Flag(FPU_FLAG, C3));
	FlagPtr flagC3 = std::make_shared<Flag>(FPU_FLAG, C3); definedFlags->insert(flagC3);

	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
	result = _result;
}


FloatingPoint_SUBFlagMacro::~FloatingPoint_SUBFlagMacro(void)
{
	/*delete firstOperand;
	delete secondOperand;
	delete result;*/
}

std::string FloatingPoint_SUBFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = SUBFLAGS_FP(" +
		(!isExpressionPointerNull(firstOperand) ? firstOperand->getExpressionString() : "NULL") + ", " + 
		(!isExpressionPointerNull(secondOperand) ? secondOperand->getExpressionString() : "NULL") + ", " + 
		(!isExpressionPointerNull(result) ? result->getExpressionString() : "NULL") + ")";	
}

//std::vector<Expression*>* FloatingPoint_SUBFlagMacro::getUsedElements(){
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

void FloatingPoint_SUBFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(firstOperand, true, usedElements_out);
	createExpressionElements(secondOperand, true, usedElements_out);
	createExpressionElements(result, true, usedElements_out);
}

InstructionPtr FloatingPoint_SUBFlagMacro::deepcopy(){
	ExpressionPtr firstOperandCopy = !isExpressionPointerNull(firstOperand) ? firstOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr secondOperandCopy = !isExpressionPointerNull(secondOperand) ? secondOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr resultCopy = !isExpressionPointerNull(result) ? result->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<FloatingPoint_SUBFlagMacro>(firstOperandCopy, secondOperandCopy, resultCopy);
}
