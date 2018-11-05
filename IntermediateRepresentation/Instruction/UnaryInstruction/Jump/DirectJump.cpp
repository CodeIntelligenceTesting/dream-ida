////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DirectJump.h"
#include <boost/lexical_cast.hpp>

DirectJump::DirectJump(int _targetBlockId) : Jump(DIRECT_JUMP)
{
	targetBlockId = _targetBlockId;
}


DirectJump::~DirectJump(void)
{
}

std::string DirectJump::getTargetString(){
	return boost::lexical_cast<std::string>(targetBlockId);
}

char* DirectJump::getTargetColoredString(){
	return getColoredString(getTargetString(), NUMBER_COLOR);
}

std::string DirectJump::getTargetCOLSTR(){
	return getCOLSTR(getTargetString(), NUMBER_COLOR);
}

InstructionPtr DirectJump::deepcopy(){
	return std::make_shared<DirectJump>(targetBlockId);
}
