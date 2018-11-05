/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "FlagMacro.h"

class SUBFlagMacro :
	public FlagMacro
{
public:
	SUBFlagMacro(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _result, bool _doesAffectCarryFlag = true);
	virtual ~SUBFlagMacro(void);

	std::string getInstructionString();
	//std::vector<Expression*>* getUsedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);
	bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	InstructionPtr deepcopy();
	std::string to_json();

	//std::vector<Flag> definedFlags;
	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	ExpressionPtr result;
	bool doesAffectCarryFlag;
};

