/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

class Expression;
class Instruction;
class LivenessAnalysis;
class LocalVariable;
class Phi_Function;
class PointerExp;
class Node;

#include <map>
#include <set>
#include <string>
#include <vector>

#include "IntermediateRepresentation/IRSharedPointers.h"
#include "DataFlowAnalysis/DataStructures/Variable.h"

class InterferenceBreaker
{
public:
	InterferenceBreaker(LivenessAnalysis* _livenessAnalyzer);
	virtual ~InterferenceBreaker(void);

	void eliminatePhiResourceInterference();
	std::map<Variable, std::set<Variable>>* getPhiCongruenceClassMap(){return &phiCongruenceClassMap;};
private:
	int copySubscript;
	LivenessAnalysis* livenessAnalyzer;
	std::map<Variable, std::set<Variable>> phiCongruenceClassMap;
	std::map<Variable, std::set<Variable>> interferenceGraph;
	std::map<Phi_FunctionPtr, std::vector<Variable>> phiFunctionToVariablesMap;
	std::map<Variable, std::set<Variable>> phiInterferenceMap;
	
	std::map<Variable, std::set<Variable>> unresolvedNeighborMap;
	std::set<Variable> candidateResourceSet;

	void initializePhiCongruenceClasses();
	void initializePhiInterferenceGraph();
	//bool isTrivialPhiFunction(Phi_Function* phiFunction);
	//bool isOnlyUsedInThisInstruction(Expression* usedExpression, Instruction* usingInstruction);
	//bool isMemoryVariable(Expression* variable);
	//bool isNotStackVariable(PointerExp* pointer);
	//void removeTrivialPhiFunction(Phi_Function* trivialPhiFunction);
	void addPhiElementsToPhiCongruenceClasses(const Phi_FunctionPtr &phiFunction/*, Node* containingNode*/);
	//void addElementsToPhiCongruenceClasses(std::vector<Expression*>* elements);

	void constructPhiVariables(const Phi_FunctionPtr &phiFunction, std::vector<Variable> &phiVariables_out);
	void clearPhiVariablesUnresolvedNeighborMap(std::vector<Variable> &phiVariables);

	bool isPhiTarget(const Phi_FunctionPtr &phiFunction, const Variable &phiVariable);
	bool doPhiCongruenceClassesInterfere(const Variable &var1, const Variable &var2);
	bool doesHaveInterferingCongruenceClasses(const Phi_FunctionPtr &phiFunction, std::vector<Variable> &phiVariables);
	//bool isPhiFunctionForMemoryVariables(Phi_Function* phiFunction);
	void breakInterference(const Phi_FunctionPtr &phiFunction, Variable &var1, Variable &var2);

	void handleUnresolvedResources();
	bool getLargestUnresolvedResourceIterator(std::set<Variable> &handledResources, Variable &unresolvedVariable_out);
	bool hasUnresolvedNeighbor(const Variable &variable);
	void markVariableAsResolved(Variable &variable);
	void removeResourcesWithResolvedNeighboursFromCandidateSet();

	void insertCopyStatementsForCandidateResources(Phi_FunctionPtr &phiFunction);
	void insertCopyForPhiSource(Phi_FunctionPtr &phiFunction, const Variable &phiSource);
	void insertCopyForPhiTarget(Phi_FunctionPtr &phiFunction);
	void insertInstructionAtBiginingOfBlock(InstructionPtr instruction, int basicBlockId);
	void insertInstructionAtEndOfBlock(InstructionPtr instruction, int basicBlockId);
	//void updateDefiningInstruction(std::string name, int subscript, Instruction* definingInstruction);
	LocalVariablePtr getNewLocalVariableFromExpression(const ExpressionPtr &exp);
	bool canRemoveOldPhiSourceFromLiveOut(const Variable &oldPhiResourceVariable);

	void updatePhiCongruenceClassMap(const Phi_FunctionPtr &phiFunction);
	void nullifyPhiCongruenceClassesWithSingletonResources();

};

