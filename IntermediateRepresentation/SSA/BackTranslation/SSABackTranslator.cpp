////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//#include "SSABackTranslator.h"
//
//#include "../../ControlFlowGraph.h"
//#include "../../../DataFlowAnalysis/Algorithms/LivenessAnalysis.h"
//#include "../../../DataFlowAnalysis/DataStructures/Variable.h"
//
//SSABackTranslator::SSABackTranslator(LivenessAnalysis* _livenessAnalyzer)
//	: livenessAnalyzer(_livenessAnalyzer)
//{
//}
//
//SSABackTranslator::~SSABackTranslator(void)
//{
//}
//
//void SSABackTranslator::eliminatePhiResourceInterference(){
//	initializePhiCongruenceClasses();
//	for(std::map<Phi_Function*, Node*>::iterator phi_iter = phiFunctionsNodeMap.begin() ; phi_iter != phiFunctionsNodeMap.end() ; ++phi_iter){
//		int containingBlockId = phi_iter->second->id;
//		std::set<Variable> candidateResourceSet;
//		std::vector<Variable> phiVariables;
//		constructPhiVariables(phi_iter->first, &phiVariables);
//		clearPhiVariablesUnresolvedNeighborMap(&phiVariables);
//		for(std::vector<Variable>::iterator xi_iter = phiVariables.begin() ; xi_iter != phiVariables.end() ; ++xi_iter){
//			for(std::vector<Variable>::iterator xj_iter = xi_iter + 1 ; xj_iter != phiVariables.end() ; ++xj_iter){
//				Variable xi = *xi_iter, xj = *xj_iter;
//				if(doPhiCongruenceClassesInterfere(&xi, &xj)){
//					std::set<Variable>* liveXi = isPhiTarget(phi_iter->first, &xi)
//													? getLiveInSetAtDefiningBlocks(phi_iter->first, &xi)
//													: getLiveOutSetAtDefiningBlocks(phi_iter->first, &xi);
//					std::set<Variable>* liveXj = isPhiTarget(phi_iter->first, &xj)
//													? getLiveInSetAtDefiningBlocks(phi_iter->first, &xj)
//													: getLiveOutSetAtDefiningBlocks(phi_iter->first, &xj);
//
//				}
//			}
//		}
//	}
//}
//
//void SSABackTranslator::initializePhiCongruenceClasses(){
//	ControlFlowGraph* controlFlowGraph = livenessAnalyzer->getControlFlowGraph();
//	for(std::map<int, Node*>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
//		for(std::vector<Instruction*>::iterator inst_iter = node_iter->second->instructions->begin() ; inst_iter != node_iter->second->instructions->end() ; ++inst_iter){
//			Instruction* currentInstruction = *inst_iter;
//			if(currentInstruction->type == PHI_FUNCTION){
//				addPhiElementsToPhiCongruenceClasses((Phi_Function*)currentInstruction, node_iter->second);
//			}
//			else{
//				break;
//			}
//		}
//	}
//}
//
//void SSABackTranslator::addPhiElementsToPhiCongruenceClasses(Phi_Function* phiFunction, Node* containingNode){
//	phiFunctionsNodeMap[phiFunction] = containingNode;
//	addElementsToPhiCongruenceClasses(phiFunction->getDefinedElements());
//	addElementsToPhiCongruenceClasses(phiFunction->getUsedElements());
//}
//
//void SSABackTranslator::addElementsToPhiCongruenceClasses(std::vector<Expression*>* elements){
//	for(std::vector<Expression*>::iterator element_iter = elements->begin() ; element_iter != elements->end() ; ++element_iter){
//		Expression* element = *element_iter;
//		Variable elementVar(element->getName(), element->subscript);
//		phiCongruenceClassMap[elementVar].insert(elementVar);
//	}
//	delete elements;
//}
//
//void SSABackTranslator::constructPhiVariables(Phi_Function* phiFunction, std::vector<Variable>* phiVariables_out){
//	for(std::map<std::string, std::map<int, std::set<int>>>::iterator name_iter = phiFunction->definingNode.begin() ; name_iter != phiFunction->definingNode.end() ; ++name_iter){
//		for(std::map<int, std::set<int>>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
//			phiVariables_out->push_back(Variable(name_iter->first, subscript_iter->first));
//		}
//	}
//}
//
//void SSABackTranslator::clearPhiVariablesUnresolvedNeighborMap(std::vector<Variable>* phiVariables){
//	for(std::vector<Variable>::iterator phiVar_iter = phiVariables->begin() ; phiVar_iter != phiVariables->end() ; ++phiVar_iter){
//		unresolvedNeighborMap[*phiVar_iter].clear();
//	}
//}
//
//
//std::set<Variable>* SSABackTranslator::getLiveInSetAtDefiningBlocks(Phi_Function* phiFunction, Variable* phiVariable){
//	std::set<Variable>* liveIn = new std::set<Variable>();
//	std::set<int> definingBlocksId = phiFunction->definingNode[phiVariable->name][phiVariable->subscript];
//	for(std::set<int>::iterator bb_iter = definingBlocksId.begin() ; bb_iter != definingBlocksId.end() ; ++bb_iter){
//		std::set<Variable> liveInAtBlock = livenessAnalyzer->liveIn[*bb_iter];
//		liveIn->insert(liveInAtBlock.begin(), liveInAtBlock.end());
//	}
//	return liveIn;
//}
//
//std::set<Variable>* SSABackTranslator::getLiveOutSetAtDefiningBlocks(Phi_Function* phiFunction, Variable* phiVariable){
//	std::set<Variable>* liveOut = new std::set<Variable>();
//	std::set<int> definingBlocksId = phiFunction->definingNode[phiVariable->name][phiVariable->subscript];
//	for(std::set<int>::iterator bb_iter = definingBlocksId.begin() ; bb_iter != definingBlocksId.end() ; ++bb_iter){
//		std::set<Variable> liveOutAtBlock = livenessAnalyzer->liveOut[*bb_iter];
//		liveOut->insert(liveOutAtBlock.begin(), liveOutAtBlock.end());
//	}
//	return liveOut;
//}
//
//bool SSABackTranslator::isPhiTarget(Phi_Function* phiFunction, Variable* phiVariable){
//	return phiFunction->target->getName().compare(phiVariable->name) == 0 && phiFunction->target->subscript == phiVariable->subscript;
//}
//
//bool SSABackTranslator::doPhiCongruenceClassesInterfere(Variable* var1, Variable* var2){
//	for(std::set<Variable>::iterator it1 = phiCongruenceClassMap[*var1].begin() ; it1 != phiCongruenceClassMap[*var1].end() ; ++it1){
//		for(std::set<Variable>::iterator it2 = phiCongruenceClassMap[*var2].begin() ; it2 != phiCongruenceClassMap[*var2].end() ; ++it2){
//			Variable x1 = *it1, x2 = *it2;
//			if(livenessAnalyzer->doInterfere(&x1, &x2))
//				return true;
//		}
//	}
//	return false;
//}
//
//void SSABackTranslator::breakInterference(Phi_Function* phiFunction, Variable* var1, Variable* var2){
//	std::set<int> firstVariableDefiningBlocks = phiFunction->definingNode[var1->name][var1->subscript];
//	std::set<int> secondVariableDefiningBlocks = phiFunction->definingNode[var2->name][var2->subscript];
//	for(std::set<int>::iterator var1Block_iter = firstVariableDefiningBlocks.begin() ; var1Block_iter != firstVariableDefiningBlocks.end() ; ++var1Block_iter){
//		for(std::set<int>::iterator var2Block_iter = secondVariableDefiningBlocks.begin() ; var2Block_iter != secondVariableDefiningBlocks.end() ; ++var2Block_iter){
//
//		}
//	}
//
//	if(isPhiTarget(phiFunction, var1)){
//		if(isPhiTarget(phiFunction, var2)){
//			breakInterferenceBetweenPhiTargets(var1, var2);
//		}
//		else{
//			breakInterferenceBetweenPhiTargetAndSource(var1, var2);
//		}
//	}
//	else{
//		if(isPhiTarget(phiFunction, var2)){
//			breakInterferenceBetweenPhiTargetAndSource(var2, var1);
//		}
//		else{
//			breakInterferenceBetweenPhiSources(var2, var1);
//		}
//	}
//}
//
//void SSABackTranslator::breakInterferenceBetweenPhiSources(Variable* source1, Variable* source2){
//
//}
//
//void SSABackTranslator::breakInterferenceBetweenPhiTargets(Variable* target1, Variable* target2){
//
//}
//
//void SSABackTranslator::breakInterferenceBetweenPhiTargetAndSource(Variable* target, Variable* source){
//
//}
