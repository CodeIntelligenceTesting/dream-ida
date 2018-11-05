////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "SUBFlagMacro.h"
#include "../BinaryInstruction/Assignment.h"
#include "../../Expression/Condition/HighLevelCondition.h"
#include "../../Expression/Constant/NumericConstant.h"

SUBFlagMacro::SUBFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result, bool _doesAffectCarryFlag) : FlagMacro(SUBFLAGS)
{
	doesAffectCarryFlag = _doesAffectCarryFlag;
	definedFlags = std::make_shared<std::set<FlagPtr>>();
	FlagPtr flagAF = std::make_shared<Flag>(CPU_FLAG, AF); definedFlags->insert(flagAF);
	if(doesAffectCarryFlag){
		FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF); definedFlags->insert(flagCF);
	}
	FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF); definedFlags->insert(flagOF);
	FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF); definedFlags->insert(flagPF);
	FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF); definedFlags->insert(flagSF);
	FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF); definedFlags->insert(flagZF);
	
	firstOperand = _firstOperand;
	secondOperand = _secondOperand;
	result = _result;
}


SUBFlagMacro::~SUBFlagMacro(void)
{
	/*delete firstOperand;
	delete secondOperand;
	delete result;*/
}

std::string SUBFlagMacro::getInstructionString(){
	//Quick-and-dirty solution
	if(definedFlags->size() == 1){
		FlagPtr flag = *(definedFlags->begin());
		if(flag->flagNo == ZF){
			if(!isExpressionPointerNull(firstOperand) && !isExpressionPointerNull(secondOperand)){
				return flag->getExpressionString() + " = " + firstOperand->getExpressionString()
					+ " == " + secondOperand->getExpressionString() + " //SUBFlagMacro";
			}
			else if(!isExpressionPointerNull(result)){
				return flag->getExpressionString() + " = " + result->getExpressionString() + " == 0 //SUBFlagMacro";
			}
		}
	}

	std::string macroString = "(";
	std::set<FlagPtr>::iterator iter;
	for(iter = definedFlags->begin() ; iter != definedFlags->end() ; iter++){
		macroString += (*iter)->getExpressionString() + ", ";
		/*if(iter + 1 != definedFlags->end()){
			macroString += ", ";
		}*/
	}
	return macroString.substr(0, macroString.length() - 2) + ") = SUBFLAGS(" + 
		(!isExpressionPointerNull(firstOperand) ? firstOperand->getExpressionString() : "NULL") + ", " + 
		(!isExpressionPointerNull(secondOperand) ? secondOperand->getExpressionString() : "NULL") + ", " + 
		(!isExpressionPointerNull(result) ? result->getExpressionString() : "NULL") + ")";
}

std::string SUBFlagMacro::to_json(){
	if(definedFlags->size() == 1){
		FlagPtr flag = *(definedFlags->begin());
		if(flag->flagNo == ZF){
			HighLevelConditionPtr cond;
			if(!isExpressionPointerNull(firstOperand) && !isExpressionPointerNull(secondOperand)){
				cond = std::make_shared<HighLevelCondition>("==", firstOperand->deepcopy(), secondOperand->deepcopy());
			}
			else{
				cond = std::make_shared<HighLevelCondition>("==", result->deepcopy(), std::make_shared<NumericConstant>(0));
			}
			AssignmentPtr assign = std::make_shared<Assignment>(flag, cond);
			return assign->to_json();
		}
	}
	return Instruction::to_json();
}

//std::vector<Expression*>* SUBFlagMacro::getUsedElements(){
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

void SUBFlagMacro::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(firstOperand, true, usedElements_out);
	createExpressionElements(secondOperand, true, usedElements_out);
	createExpressionElements(result, true, usedElements_out);
}

bool SUBFlagMacro::replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool replacementResult = false;
	if(!isExpressionPointerNull(firstOperand)){
		if(firstOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete firstOperand;
			firstOperand = replacingExpression->deepcopy();
			replacementResult = true;
		}
	}
	if(!isExpressionPointerNull(secondOperand)){
		if(secondOperand->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete secondOperand;
			secondOperand = replacingExpression->deepcopy();
			replacementResult = true;
		}
	}
	if(!isExpressionPointerNull(result)){
		if(result->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			//delete result;
			result = replacingExpression->deepcopy();
			replacementResult = true;
		}
	}
	return replacementResult;
}

InstructionPtr SUBFlagMacro::deepcopy(){
	ExpressionPtr firstOperandCopy = !isExpressionPointerNull(firstOperand) ? firstOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr secondOperandCopy = !isExpressionPointerNull(secondOperand) ? secondOperand->deepcopy() : std::shared_ptr<Expression>();
	ExpressionPtr resultCopy = !isExpressionPointerNull(result) ? result->deepcopy() : std::shared_ptr<Expression>();
	return std::make_shared<SUBFlagMacro>(firstOperandCopy, secondOperandCopy, resultCopy, doesAffectCarryFlag);
}
