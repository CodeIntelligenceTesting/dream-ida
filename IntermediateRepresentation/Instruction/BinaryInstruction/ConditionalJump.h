/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

#include "BinaryInstruction.h"
#include "../../Expression/Condition/HighLevelCondition.h"

class ConditionalJump :
	public BinaryInstruction
{
public:
	ConditionalJump(ExpressionPtr _condition, int _target);
	virtual ~ConditionalJump(void);

	std::string getInstructionString();
	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	/*std::vector<Expression*>* getUsedElements();
	std::vector<Expression*>* getDefinedElements();*/
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);

	void replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	InstructionPtr deepcopy();

	ExpressionPtr condition;
	int target;
};

