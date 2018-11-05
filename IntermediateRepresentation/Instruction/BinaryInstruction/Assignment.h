/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "BinaryInstruction.h"

class Assignment :
	public BinaryInstruction
{
public:
	Assignment(ExpressionPtr _lhsOperand, ExpressionPtr _rhsOperand);
	virtual ~Assignment(void);

	std::string getInstructionString();
	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	std::string to_json();
	/*std::vector<Expression*>* getUsedElements();
	std::vector<Expression*>* getDefinedElements();*/
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);
	void getDefinedElements(std::vector<ExpressionPtr>& definedElements_out);

	void replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	InstructionPtr deepcopy();

	ExpressionPtr lhsOperand;
	ExpressionPtr rhsOperand;

private:
	bool replaceOperand(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression, ExpressionPtr& operand);
};

