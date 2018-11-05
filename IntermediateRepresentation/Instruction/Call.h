/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include <vector>

#include "Instruction.h"
#include "../IRSharedPointers.h"

class Call :
	public Instruction
{
public:
	Call(ExpressionPtr _functionPointer, ExpressionVectorPtr _parameters, ExpressionVectorPtr _returns);
	virtual ~Call(void);

	std::string getInstructionString();
	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	/*std::vector<Expression*>* getUsedElements();
	std::vector<Expression*>* getDefinedElements();*/
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);
	void getDefinedElements(std::vector<ExpressionPtr>& definedElements_out);
	void replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	std::string to_json();

	InstructionPtr deepcopy();

	//std::string functionName;
	ExpressionPtr functionPointer;
	ExpressionVectorPtr parameters;
	ExpressionVectorPtr returns;

private:
	bool replaceElementInList(std::string nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression, ExpressionVectorPtr& containingList);
};

