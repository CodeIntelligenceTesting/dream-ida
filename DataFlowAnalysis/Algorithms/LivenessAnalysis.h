/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "DataFlowAlgorithm.h"

class Phi_Function;
class Variable;

class LivenessAnalysis :
	public DataFlowAlgorithm
{
public:
	LivenessAnalysis(ControlFlowGraphPtr _controlFlowGraph,
					  definitionsMapPtr _definitionsMap,
					  //std::map<std::string, std::map<int, Instruction*>>* _flagDefinitionsMap,
					  usesMapPtr _usesMap);

	virtual ~LivenessAnalysis(void);
	std::map<Variable, std::set<Variable>> interferenceGraph;
	std::map<int, std::set<Variable>> liveOut;
	std::map<int, std::set<Variable>> liveIn;

	void applyAlgorithm();

	bool doInterfere(const Variable &var1, const Variable &var2);
	bool isIntersectionWithLiveOutEmpty(int basicBlockId, std::set<Variable> &variables);
	bool isIntersectionWithLiveInEmpty(int basicBlockId, std::set<Variable> &variables);
	bool isLiveAtBlockEntrance(const Variable &variable, int basicBlockId);
	bool isLiveAtBlockExit(const Variable &variable, int basicBlockId);

	void addToLiveIn(const Variable &varToAdd);
	void removeFromLiveIn(const Variable &varToRemove);
	void addToLiveOut(const Variable &varToAdd);
	void removeFromLiveOut(const Variable &varToRemove);

	void addInterferencesWithLiveIn(const Variable &interferingVariable);
	void addInterferencesWithLiveOut(const Variable &interferingVariable);

	void replaceInterferingMamoryLocations();
	bool doesInterfereWithOtherVersions(const ExpressionPtr &variable);
	
	void replaceVariables(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable);

private:
	std::map<InstructionPtr, ExpressionVectorPtr> instructionToDefinedElementsMap;
	ExpressionVectorPtr getDefinedElements(const InstructionPtr &definingInstruction);

	std::set<int> handledBasicBlocks;
	int memorySubscript;

	void computeVariableLiveness(const Variable &variable);

	void addPhiTargetsToLiveIn();
	void addPhiTargetsToBasicBlockLiveIn(int basicBlockId, const NodePtr &basicBlock);
	void addVariablesToBasicBlockLiveIn(int basicBlockId, ExpressionVectorPtr variables);

	bool isBlockHandled(int basicBlockId);
	bool areNotAliases(const Variable &v1, const Variable &v2);
	bool isIntersectionEmpty(std::set<Variable> &set1, std::set<Variable> &set2);

	void makeLiveInAtInstruction(const InstructionPtr &instruction, const Variable &variable);
	void makeLiveOutAtInstruction(const InstructionPtr &instruction, const Variable &variable);
	void makePhiArgumentsLiveOutAtBlock(const Phi_FunctionPtr &phiFunction, const Variable &variable);
	void makeLiveOutAtBlock(int basicBlockId, const Variable &variable);
	void addToInterferenceGraph(const Variable &v1, const Variable &v2);

	void addToSet(std::set<Variable> &targetSet, const Variable &varToAdd);
	void removeFromSet(std::set<Variable>& targetSet, const Variable &varToRemove);
	void addInterferencesWithSet(std::set<Variable> &targetSet, const Variable &interferingVariable);

	//void addCopyStatement(Expression* expToCopy, Instruction* afterThisInstruction);

	void replaceUses(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable);
	void replaceInDefinitionsMap(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable);
	void replaceInUsesMap(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable);
	void replaceInLiveMap(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable, std::map<int, std::set<Variable>> &liveMap);
	void replaceInInterferenceGraph(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable);
	bool isInPhiFunctionWithInterferingResources(const ExpressionPtr &exp);
	Phi_FunctionPtr getRelatedPhiFunction(const ExpressionPtr & exp);
	bool doPhiResourcesInterfere(const Phi_FunctionPtr &phiFunction);
	//bool isPhiFunctionOfMemoryVariables(const Phi_FunctionPtr &phiFunction);
	LocalVariablePtr getCorrespondingLocalVariable(const ExpressionPtr &exp);
	void removeSubscriptsFromUses(ExpressionPtr &exp);
};

