/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once


#include <algorithm>

#include "DataFlowAlgorithm.h"
#include <fstream>

class Phi_Function;
//class Expression;


class DeadCodeElimination :
	public DataFlowAlgorithm
{
public:
	DeadCodeElimination(ControlFlowGraphPtr _controlFlowGraph,
						definitionsMapPtr _definitionsMap,
						definitionsMapPtr _flagDefinitionsMap,
						usesMapPtr _usesMap);
	virtual ~DeadCodeElimination(void);

	void applyAlgorithm();
private:
	void removeDefinitionIfDead(const std::string &definitionName, int definitionSubscript);
	void removeDefinitionFromFunctionReturns(const std::string &definitionName, int definitionSubscript, CallPtr &definingFunctionCall);
	void removeDeadFlag(const std::string &deadFlagName, int deadFlagSubscript, FlagMacroPtr &definingFlagMacro);
	void removeDeadDefinition(const std::string &deadDefinitionName, int deadDefinitionSubscript, const InstructionPtr &definingInstruction);

	bool canRemoveExpression(const ExpressionPtr &exp);
	bool canRemoveInstruction(const InstructionPtr &inst);
	bool isNeitherESPNorConstant(const ExpressionPtr &exp);

	void removeInstruction(const InstructionPtr &instToBeDeleted);
	void removeExpressionsFromUsingInstruction(std::vector<ExpressionPtr> &expListToBeRemoved, const InstructionPtr &parentInst);
	void removeInstructionFromUsesMap(const InstructionPtr &instToBeDeleted);
	void updateRelatedRegistersDefinitionsMap_Remove(const std::string &name, int subscript);
	
	void removePhiArgumentsSameAsTarget();

	bool removeTrivialPhiChains();
	/*bool isMemoryVariable(Expression* variable);
	bool isNotStackVariable(PointerExp* pointer);*/
	bool isInTrivialPhiChain(const InstructionPtr& inst, std::set<InstructionPtr>& chainElements);
	bool isPhiFunctionWithEqualParameters(const Phi_FunctionPtr& phi);
	bool isInList(const ExpressionPtr& var, std::vector<ExpressionPtr>& varList);
	void replacePhiFunctionByVariable(const Phi_FunctionPtr& phi, ExpressionPtr& var);
	bool isCopyStatementOfLocalVariables(const InstructionPtr& inst);
	bool isOnlyUsedInPhiFunctionsOrCopyStatements(const ExpressionPtr &variable);
	bool isCopyStatement(const InstructionPtr &inst);
	bool isESPIncDec(const AssignmentPtr &assign);

	std::ofstream dce_out;
};

