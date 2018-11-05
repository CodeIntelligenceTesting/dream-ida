////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DataFlowAnalyzer.h"

#include "../Shared/GraphViewer.h"
#include "Algorithms/DeadCodeElimination.h"
#include "Algorithms/ConditionCodePropagation.h"
#include "Algorithms/ExpressionPropagation.h"
#include "Algorithms/LivenessAnalysis.h"
#include "Algorithms/FunctionArgumentsDetector.h"
#include "../IntermediateRepresentation/SSA/BackTranslation/InterferenceBreaker.h"
#include "../IntermediateRepresentation/SSA/BackTranslation/PhiFunctionsEliminator.h"

#include "DataStructures/Variable.h"

#include <fstream> //To Be deleted
#include <time.h>

DataFlowAnalyzer::DataFlowAnalyzer(ControlFlowGraphPtr _controlFlowGraph,
								   definitionsMapPtr _definitionsMap,
								   definitionsMapPtr _flagDefinitionsMap,
								   usesMapPtr _usesMap)
{
	controlFlowGraph = _controlFlowGraph;
	definitionsMap = _definitionsMap;
	flagDefinitionsMap = _flagDefinitionsMap;
	usesMap = _usesMap;
}


DataFlowAnalyzer::~DataFlowAnalyzer(void)
{
}

void DataFlowAnalyzer::applyDataFlowAnalysisFirstRound(){
	//printUsesMap();
	//printDefinitionsMap();

	ConditionCodePropagation ccp(controlFlowGraph, definitionsMap, flagDefinitionsMap, usesMap);
	ccp.applyAlgorithm();
	
	ExpressionPropagation ep(controlFlowGraph, definitionsMap, usesMap/*, phiVariables*/);
	

	//printUsesMap();
	//printDefinitionsMap();
	DeadCodeElimination dce(controlFlowGraph, definitionsMap, flagDefinitionsMap, usesMap);
	dce.applyAlgorithm();
	ep.applyAlgorithmNoMemoryVariables();
	dce.applyAlgorithm();
}

void DataFlowAnalyzer::applyDataFlowAnalysisSecondRound(){
	ExpressionPropagation ep(controlFlowGraph, definitionsMap, usesMap);
	ep.applyAlgorithmWithMemoryVariables();
	
	DeadCodeElimination dce(controlFlowGraph, definitionsMap, flagDefinitionsMap, usesMap);
	dce.applyAlgorithm();

	//printUsesMap();
	//printDefinitionsMap();

	FunctionArgumentsDetector functionArgumentsDetector(controlFlowGraph, definitionsMap, usesMap);
	functionArguments = functionArgumentsDetector.getArguments();
}

void DataFlowAnalyzer::applyLivenessAnalysis(){
	LivenessAnalysis livenessAnalysis(controlFlowGraph, definitionsMap, usesMap);
	livenessAnalysis.applyAlgorithm();
}

void DataFlowAnalyzer::translateOutOfSSA(TypeMapPtr typeMap){
	LivenessAnalysis livenessAnalysis(controlFlowGraph, definitionsMap, usesMap);
	livenessAnalysis.applyAlgorithm();
	livenessAnalysis.replaceInterferingMamoryLocations();
	InterferenceBreaker interferenceBreaker(&livenessAnalysis);
	interferenceBreaker.eliminatePhiResourceInterference();
	PhiFunctionsEliminator phiFunctionsEliminator(controlFlowGraph, definitionsMap, usesMap, typeMap, interferenceBreaker.getPhiCongruenceClassMap(), functionArguments);
	phiFunctionsEliminator.eliminatePhiFunctions();
}


void DataFlowAnalyzer::unifyRegisterUses(){
	
}

void DataFlowAnalyzer::printUsesMap(){
	std::ofstream out;
	out.open("e:\\usesMap.txt");
	for(std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator niter = usesMap->begin() ; niter != usesMap->end() ; ++niter){
		for(std::map<int, std::vector<InstructionPtr>>::iterator siter = niter->second.begin() ; siter != niter->second.end() ; ++siter){
			if(!siter->second.empty()){
				out << "uses(" << niter->first.c_str() << "_" << siter->first << "):" << std::endl;
				for(std::vector<InstructionPtr>::iterator iter = siter->second.begin() ; iter != siter->second.end() ; ++iter){
					out << "\t" << (*iter)->getInstructionString().c_str() << std::endl;
				}
			}
		}
	}
	out.close();
}

void DataFlowAnalyzer::printDefinitionsMap(){
	std::ofstream out;
	out.open("e:\\definitionsMap.txt");
	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator niter = definitionsMap->begin() ; niter != definitionsMap->end() ; ++niter){
		for(std::map<int, InstructionPtr>::iterator siter = niter->second.begin() ; siter != niter->second.end() ; ++siter){
			if(siter->second.get() != NULL){
				out << "definition (" << niter->first.c_str() << "_" << siter->first << "): " << (siter->second != NULL ? siter->second->getInstructionString().c_str() : "NULL") << std::endl;
				//msg("definition(%s_%d) = %d\n", niter->first.c_str(), siter->first, siter->second);
				//msg("definition(%s_%d) = %s\n", niter->first.c_str(), siter->first, siter->second->getInstructionString().c_str());
				out.flush();
			}
		}
	}
}
