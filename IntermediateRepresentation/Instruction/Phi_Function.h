/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <set>
#include <vector>

#include "Instruction.h"


class Phi_Function :
	public Instruction
{
public:
	Phi_Function(ExpressionPtr _target, int argNum);
	virtual ~Phi_Function(void);

	std::string getInstructionString();
	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	/*std::vector<Expression*>* getUsedElements();
	std::vector<Expression*>* getDefinedElements();*/
	void getUsedElements(std::vector<ExpressionPtr>& usedElements_out);
	void getDefinedElements(std::vector<ExpressionPtr>& definedElements_out);

	bool replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	InstructionPtr deepcopy();

	ExpressionPtr target;
	ExpressionVectorPtr arguments;
	//std::map<std::string, std::map<int, std::set<int>>> definingNode;
};

