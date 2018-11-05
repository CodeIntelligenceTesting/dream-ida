////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "IndirectTableJump.h"

#include "../../../../Shared/ExpressionUtilities.h"

IndirectTableJump::IndirectTableJump(ExpressionPtr _targetBlock, ExpressionPtr _jumpTableIndex, ExpressionPtr _switchVariable, nodeToCasesMapPtr _nodeToCasesMap, int _defaultBlockId)
	: IndirectJump(_targetBlock, INDIRECT_TABLE_JUMP)
{
	jumpTableIndex = _jumpTableIndex;
	switchVariable = _switchVariable;
	nodeToCasesMap = _nodeToCasesMap;
	defaultBlockId = _defaultBlockId;
}


IndirectTableJump::~IndirectTableJump(void)
{
	//delete jumpTableIndex; jumpTableIndex = NULL;
	//delete caseToBlockMap; caseToBlockMap = NULL;
}

std::string IndirectTableJump::getInstructionString(){
	if(switchVariable.get() != NULL)
		return "switch(" + switchVariable->getExpressionString() + ")";
	else
		return "jump " + targetBlock->getExpressionString();
}

std::string IndirectTableJump::getInstructionCOLSTR(){
	if(switchVariable.get() != NULL)
		return std::string(COLSTR("switch", SCOLOR_MACRO)) + "(" + switchVariable->getExpressionCOLSTR() + ")";
	else
		return std::string(COLSTR("jump", SCOLOR_MACRO)) + targetBlock->getExpressionCOLSTR();
}

void IndirectTableJump::getUsedElements(std::vector<ExpressionPtr>& usedElements_out){
	if(switchVariable.get() != NULL)
		createExpressionElements(switchVariable, true, usedElements_out);
	else
		createExpressionElements(targetBlock, true, usedElements_out);
}

InstructionPtr IndirectTableJump::deepcopy(){
	nodeToCasesMapPtr nodeToCasesMapCopy = std::make_shared<std::map<int, std::set<int>>>();
	for(std::map<int, std::set<int>>::iterator node_iter = nodeToCasesMap->begin() ; node_iter != nodeToCasesMap->end() ; ++node_iter){
		nodeToCasesMapCopy->insert(std::pair<int, std::set<int>>(node_iter->first, node_iter->second));
	}
	return std::make_shared<IndirectTableJump>(targetBlock->deepcopy(), jumpTableIndex->deepcopy(), switchVariable->deepcopy(), nodeToCasesMapCopy, defaultBlockId);
}
