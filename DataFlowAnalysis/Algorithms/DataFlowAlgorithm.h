/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <string>
#include <vector>

#include "../../IntermediateRepresentation/IRHeaders.h"

class DataFlowAlgorithm
{
public:
	DataFlowAlgorithm(ControlFlowGraphPtr _controlFlowGraph,
					  definitionsMapPtr _definitionsMap,
					  definitionsMapPtr _flagDefinitionsMap,
					  usesMapPtr _usesMap);
	virtual ~DataFlowAlgorithm(void);

	virtual void applyAlgorithm(){};
	//std::vector<Expression*>* getFunctionArguments();
	ControlFlowGraphPtr getControlFlowGraph(){return controlFlowGraph;};

	void addInstructionToUsesMap(const ExpressionPtr& usedVariable, InstructionPtr instructionToAdd);
	void deleteInstructionFromUsesMap(const ExpressionPtr& unusedVariable, InstructionPtr instructionToRemove);
	void updateDefinitionsMap(const ExpressionPtr& definedVariable, InstructionPtr definingInstruction);

	/*Instruction* getDefiningInstruction(std::string variableName, int VariableSubscript);
	std::vector<Instruction*>* getUsingInstructions(std::string variableName, int VariableSubscript);*/

	InstructionPtr getDefiningInstruction(const std::string& variableName, int variableSubscript);
	std::vector<InstructionPtr>* getUsingInstructions(const std::string& variableName, int variableSubscript);//NOT used
	std::vector<InstructionPtr>* getUsingInstructions(const ExpressionPtr& variable);
	//void clearUses(Expression* exp);//NotUsed

protected:
	ControlFlowGraphPtr controlFlowGraph;
	definitionsMapPtr definitionsMap;
	definitionsMapPtr flagDefinitionsMap;
	usesMapPtr usesMap;
	std::map<std::string, std::set<std::string>> relatedRegistersMap;

	void constructRelatedRegistersMap();

	/*bool canRemoveExpression(Expression* exp);
	bool canRemoveInstruction(Instruction* inst);
	bool isNeitherESPNorConstant(Expression* exp);*/

	bool noRelatedRegisterUsed(const std::string& name, int subscript);
	
	/*void removeInstruction(Instruction* instToBeDeleted);
	void removeInstructionFromUsesMap(Instruction* instToBeDeleted);
	void updateRelatedRegistersDefinitionsMap_Remove(std::string name, int subscript);*/

	//void removeDeadDefinition(std::string deadDefinitionName, int deadDefinitionSubscript, Instruction* definingInstruction);
	
	//void addExpressionsToUsingInstruction(std::vector<Expression*>* expListToBeAdded, Instruction* parentInst);
	//void removeExpressionsFromUsingInstruction(std::vector<Expression*>* expListToBeRemoved, Instruction* parentInst);
	void removeInstructionFromVariableUsesMap(const std::string& variableName, int variableSubscript, const InstructionPtr &instructionToRemove);

	//void removeDefinitionIfDead(std::string definitionName, int definitionSubscript);
	//void removeDeadFlag(std::string deadFlagName, int deadFlagSubscript, FlagMacro* definingFlagMacro);
	//void removeDefinitionFromFunctionReturns(std::string definitionName, int definitionSubscript, Call* definingFunctionCall);

	bool isVariableDead(const std::string& variableName, int variableSubscript);
	bool isVariableDefined(const std::string& variableName, int variableSubscript);

	bool isInExpressionList(std::vector<ExpressionPtr> &expr_list, const ExpressionPtr& expr);

	//void removePhiArgumentsSameAsTarget();
};

