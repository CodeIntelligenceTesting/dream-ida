/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

class ControlFlowGraph;
class DataFlowAngorithm;
class Expression;
class Instruction;
class RelatedRegistersMonitor;
class Variable;

#include <map>
#include <set>
#include <vector>
#include "../../IRSharedPointers.h"
#include "../../../TypeAnalysis/TypeSharedPointers.h"

typedef std::shared_ptr<RelatedRegistersMonitor> RelatedRegistersMonitorPtr;

class PhiFunctionsEliminator
{
public:
	PhiFunctionsEliminator(ControlFlowGraphPtr _controlFlowGraph,
						   definitionsMapPtr _definitionsMap,
						   usesMapPtr _usesMap,
						   TypeMapPtr _typeMap,
						   std::map<Variable, std::set<Variable>>* _phiCongruenceClassMap,
						   ExpressionVectorPtr _functionArguments);
	virtual ~PhiFunctionsEliminator(void);

	void eliminatePhiFunctions();
private:
	ControlFlowGraphPtr controlFlowGraph;
	definitionsMapPtr definitionsMap;
	usesMapPtr usesMap;
	TypeMapPtr typeMap;
	std::map<Variable, std::set<Variable>>* phiCongruenceClassMap;
	RelatedRegistersMonitorPtr relatedRegistersMonitor;
	ExpressionVectorPtr functionArguments;

	int phiRepresentativeSubscript;

	bool isVariableDefined(const Variable &var);
	bool isVariableUsed(const Variable &var);
	void removeInstruction(const InstructionPtr &instructionToRemove);

	void eliminatePhiFunctionsWithMemoryVariables();
	void replaceMemoryVariable(const Variable &var);

	void eliminatePhiFunctionsWithSimpleVariables();
	bool replaceSimpleVariable(const Variable &var, const ExpressionPtr &renamedVariable);
	void updateFunctionArguments(const Variable &var, const ExpressionPtr &renamedVariable);

	bool isMemoryVariable(const Variable &var);
	bool isMemoryVariable(const Variable &var, const InstructionPtr &definingInstruction);
	void removeSubscriptOfDefintion(const Variable &var, InstructionPtr &inThisInstruction);
	void removeSubscriptOfUse(const Variable &var, InstructionPtr &inThisInstruction);
	void removeSubscriptFromCandidates(const Variable &var, std::vector<ExpressionPtr> &candidates);
	void removeZeroSubscripts();

	TypePtr getphiCongruenceClassType(std::set<Variable>& phiCongruenceClass);
};

