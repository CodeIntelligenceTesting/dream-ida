////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "IndirectJump.h"
#include "../../../../Shared/ExpressionUtilities.h"



IndirectJump::IndirectJump(ExpressionPtr _targetBlock, InstructionType _instructionType) : Jump(_instructionType)
{
	targetBlock = _targetBlock;
}


IndirectJump::~IndirectJump(void)
{
	//delete targetBlock; targetBlock = NULL;
}

std::string IndirectJump::getTargetString(){
	return targetBlock->getExpressionString();
}


char* IndirectJump::getTargetColoredString(){
	return targetBlock->getColoredExpressionString();
}

std::string IndirectJump::getTargetCOLSTR(){
	return targetBlock->getExpressionCOLSTR();
}

//std::vector<Expression*>* IndirectJump::getUsedElements(){
//	return targetBlock->getExpressionElements(true);
//}


//std::vector<Expression*>* IndirectJump::getDefinedElements(){
//	return NULL;
//}

void IndirectJump::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	createExpressionElements(targetBlock, true, usedElements_out);
}

InstructionPtr IndirectJump::deepcopy(){
	return std::make_shared<IndirectJump>(targetBlock->deepcopy(), type);
}
