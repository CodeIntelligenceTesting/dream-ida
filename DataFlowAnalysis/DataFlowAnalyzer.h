/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <vector>

#include "../IntermediateRepresentation/IRHeaders.h"
#include "../TypeAnalysis/TypeSharedPointers.h"

class LivenessAnalysis;
class Variable;

class DataFlowAnalyzer
{
public:
	DataFlowAnalyzer(ControlFlowGraphPtr _controlFlowGraph,
					 definitionsMapPtr _definitionsMap,
					 definitionsMapPtr _flagDefinitionsMap,
					 usesMapPtr _usesMap
					 /*std::set<Variable>* _phiVariables*/);
	virtual ~DataFlowAnalyzer(void);

	void applyDataFlowAnalysisFirstRound();
	void applyDataFlowAnalysisSecondRound();
	void applyLivenessAnalysis();
	void translateOutOfSSA(TypeMapPtr typeMap);

	void printUsesMap(); //To be deleted
	void printDefinitionsMap(); //To be deleted

	ExpressionVectorPtr functionArguments;
private:
	definitionsMapPtr definitionsMap;
	definitionsMapPtr flagDefinitionsMap;
	usesMapPtr usesMap;
	//std::set<Variable>* phiVariables;
	ControlFlowGraphPtr controlFlowGraph;
	void unifyRegisterUses();
};
