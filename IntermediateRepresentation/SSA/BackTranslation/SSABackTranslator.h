/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
//#pragma once
//
//#include <map>
//#include <set>
//#include <vector>
//
//class ControlFlowGraph;
//class Expression;
//class LivenessAnalysis;
//class Node;
//class Phi_Function;
//class Variable;
//
//class SSABackTranslator
//{
//public:
//	SSABackTranslator(LivenessAnalysis* _livenessAnalyzer);
//	virtual ~SSABackTranslator(void);
//
//private:
//	void eliminatePhiResourceInterference();
//
//	void initializePhiCongruenceClasses();
//	void addPhiElementsToPhiCongruenceClasses(Phi_Function* phiFunction, Node* containingNode);
//	void addElementsToPhiCongruenceClasses(std::vector<Expression*>* elements);
//
//	void constructPhiVariables(Phi_Function* phiFunction, std::vector<Variable>* phiVariables_out);
//	void clearPhiVariablesUnresolvedNeighborMap(std::vector<Variable>* phiVariables);
//	
//	std::set<Variable>* getLiveInSetAtDefiningBlocks(Phi_Function* phiFunction, Variable* phiVariable);
//	std::set<Variable>* getLiveOutSetAtDefiningBlocks(Phi_Function* phiFunction, Variable* phiVariable);
//	bool isPhiTarget(Phi_Function* phiFunction, Variable* phiVariable);
//	bool doPhiCongruenceClassesInterfere(Variable* var1, Variable* var2);
//	
//	void breakInterference(Phi_Function* phiFunction, Variable* var1, Variable* var2);
//	void breakInterferenceBetweenPhiSources(Variable* source1, Variable* source2);
//	void breakInterferenceBetweenPhiTargets(Variable* target1, Variable* target2);
//	void breakInterferenceBetweenPhiTargetAndSource(Variable* target, Variable* source);
//
//	LivenessAnalysis* livenessAnalyzer;
//	std::map<Variable, std::set<Variable>> phiCongruenceClassMap;
//	std::map<Phi_Function*, Node*> phiFunctionsNodeMap;
//	std::map<Variable, std::set<Variable>> unresolvedNeighborMap;
//};
