////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "SALFlagMacro.h"


SALFlagMacro::SALFlagMacro(ExpressionPtr _operand, ExpressionPtr _count, ExpressionPtr _result) : FlagMacro(SALFLAGS)
{
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF); definedFlags->insert(flagOF);

	operand = _operand;
	count = _count;
	result = _result;
}


SALFlagMacro::~SALFlagMacro(void)
{
	/*delete operand;
	delete count;
	delete result;*/
}

std::string SALFlagMacro::getInstructionString(){
	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
	}
	return macroString.substr(0, macroString.length() - 2) + ") = SALFLAGS("
			+ operand->getExpressionString() + ", " + count->getExpressionString()
			+ ", " + result->getExpressionString() + ")";
}

//std::vector<Expression*>* SALFlagMacro::getUsedElements(){
//	std::vector<Expression*>* usedElements = new std::vector<Expression*>();
//	if(operand != NULL){
//		std::vector<Expression*>* operandElements = operand->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), operandElements->begin(), operandElements->end());
//		delete operandElements;
//	}
//	if(count != NULL){
//		std::vector<Expression*>* countOperand = count->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), countOperand->begin(), countOperand->end());
//		delete countOperand;
//	}
//	if(result != NULL){
//		std::vector<Expression*>* resultElements = result->getExpressionElements(true);
//		usedElements->insert(usedElements->end(), resultElements->begin(), resultElements->end());
//		delete resultElements;
//	}
//	return usedElements;
//}

void SALFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(operand, true, usedElements_out);
	createExpressionElements(count, true, usedElements_out);
	createExpressionElements(result, true, usedElements_out);
}

InstructionPtr SALFlagMacro::deepcopy(){
	ExpressionPtr operandCopy = !isExpressionPointerNull(operand) ? operand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr countCopy = !isExpressionPointerNull(count) ? count->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr resultCopy = !isExpressionPointerNull(result) ? result->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<SALFlagMacro>(operandCopy, countCopy, resultCopy);
}
