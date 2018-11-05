/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include <vector>

//To be deleted
#include <ida.hpp>
#include <idp.hpp>

#include "InstructionTypes.h"
#include "../Expression/Expression.h"
#include "../Enumerations/CodeColors.h"
#include "../../Shared/ColoredStrings.h"
#include "../../IntermediateRepresentation/IRSharedPointers.h"

//#include "../../DataFlowAnalysis/MathematicalSimplification/SimplificationFeasibilityChecks.h"
//#include "../../DataFlowAnalysis/MathematicalSimplification/localMathematicalSimplifications.h"

class Instruction
{
public:
	//Abstract base class for all instructions.
	Instruction(InstructionType _type);
	virtual ~Instruction(void);

	virtual std::string getInstructionString() = 0;
	virtual char* getColoredInstructionString() = 0;
	virtual std::string getInstructionCOLSTR() = 0;
	virtual std::string to_json(){return "{\"instruction_type\":\"LOWLEVEL\"}";}
	virtual void replaceUsedChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){};
	virtual bool replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){return false;};
	virtual bool replaceUse(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){return false;};

	virtual InstructionPtr deepcopy() = 0;
	
	/*virtual std::vector<Expression*>* getUsedElements() = 0;
	virtual std::vector<Expression*>* getDefinedElements() = 0;
	std::vector<Expression*>* getAllElements();*/
	virtual void getUsedElements(std::vector<ExpressionPtr>& usedElements_out) = 0;
	virtual void getDefinedElements(std::vector<ExpressionPtr>& definedElements_out){};
	void getAllElements(std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr getDefinedExpression(const std::string& name, int subscript);
	void getUsesOfExpression(const std::string& name, int subscript, std::vector<ExpressionPtr>& uses);


	virtual bool isOverwritingStatement();
	virtual void replaceBySimplifiedExpression(ExpressionPtr& expressionToSimplifyPtr);

	unsigned int getNumberOfUsedExpressions();

	InstructionType type;
	int ContainingNodeId;
	long address;
private:
	ExpressionPtr getExpressionFromVector(const std::string& name, int subscript, std::vector<ExpressionPtr>& expressionsVector);
};
