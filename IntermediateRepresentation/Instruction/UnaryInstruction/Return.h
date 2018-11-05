/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "UnaryInstruction.h"

class Return :
	public UnaryInstruction
{
public:
	Return(ExpressionPtr _operand);
	virtual ~Return(void);

	std::string getInstructionString();
	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	//std::vector<Expression*>* getUsedElements();
	//std::vector<Expression*>* getDefinedElements();
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);
	void replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	std::string to_json();

	InstructionPtr deepcopy();

	ExpressionPtr operand;
};

