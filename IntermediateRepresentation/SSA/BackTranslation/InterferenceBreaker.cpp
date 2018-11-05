////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "InterferenceBreaker.h"

#include "IntermediateRepresentation/ControlFlowGraph.h"
#include "DataFlowAnalysis/Algorithms/LivenessAnalysis.h"
#include "DataFlowAnalysis/DataStructures/Variable.h"

#include <time.h>

InterferenceBreaker::InterferenceBreaker(LivenessAnalysis* _livenessAnalyzer)
						: livenessAnalyzer(_livenessAnalyzer)
{
	copySubscript = 0;
}


InterferenceBreaker::~InterferenceBreaker(void)
{
}

void InterferenceBreaker::eliminatePhiResourceInterference(){
	//livenessAnalyzer->replaceInterferingMamoryLocations();
	//time_t t0 = time(NULL);
	initializePhiCongruenceClasses();
	//msg("initializePhiCongruenceClasses %f\n", difftime(time(NULL), t0));
	//t0 = time(NULL);
	initializePhiInterferenceGraph();
	//msg("initializePhiInterferenceGraph %f\n", difftime(time(NULL), t0));

	for(std::map<Phi_FunctionPtr, std::vector<Variable>>::iterator phi_iter = phiFunctionToVariablesMap.begin() ; phi_iter != phiFunctionToVariablesMap.end() ; ++phi_iter){
		candidateResourceSet.clear();
		Phi_FunctionPtr currentPhiFunction = phi_iter->first;
		
		std::vector<Variable> phiVariables = phi_iter->second;//&(phiFunctionToVariablesMap[currentPhiFunction]);
		
		//constructPhiVariables(currentPhiFunction, phiVariables);
		unresolvedNeighborMap.clear();
		//clearPhiVariablesUnresolvedNeighborMap(&phiVariables);
		

		//msg("--------\ncurrentPhiFunction: %s\n", currentPhiFunction->getInstructionString().c_str());
		if(doesHaveInterferingCongruenceClasses(currentPhiFunction, phiVariables)){
			for(std::vector<Variable>::iterator xi_iter = phiVariables.begin() ; xi_iter != phiVariables.end() ; ++xi_iter){
				for(std::vector<Variable>::iterator xj_iter = xi_iter + 1 ; xj_iter != phiVariables.end() ; ++xj_iter){
					Variable xi = *xi_iter, xj = *xj_iter;
					//msg("interfere(%s_%d, %s_%d)", xi.name.c_str(), xi.subscript, xj.name.c_str(), xj.subscript);
					if(doPhiCongruenceClassesInterfere(xi, xj)){
						//msg(".... yes");
						//msg("interfere(%s_%d, %s_%d)", xi.name.c_str(), xi.subscript, xj.name.c_str(), xj.subscript);
						breakInterference(currentPhiFunction, xi, xj);
					}
					//msg("\n");
					//time_t t1 = time(NULL);

				}
			}
		}
		//msg("candidateResourceSet.size() = %d\n", candidateResourceSet.size());
		//msg("unresolvedNeighborMap.size() = %d\n", unresolvedNeighborMap.size());
		handleUnresolvedResources();
		insertCopyStatementsForCandidateResources(currentPhiFunction);
		updatePhiCongruenceClassMap(currentPhiFunction);
	}
	//msg("timediff = %f\n", difftime(time(NULL), t0));
	//msg("tmp_numberInterferenceTest = %d\n", tmp_numberInterferenceTest);
	nullifyPhiCongruenceClassesWithSingletonResources();
}

void InterferenceBreaker::initializePhiCongruenceClasses(){
	/*ControlFlowGraph* controlFlowGraph = livenessAnalyzer->getControlFlowGraph();
	for(std::map<Phi_Function*, std::set<Variable>>::iterator phi_iter = livenessAnalyzer->phiFunctionInterferenceMap.begin();
		phi_iter != livenessAnalyzer->phiFunctionInterferenceMap.end() ; ++phi_iter){
			int blockId = phi_iter->first->target->phiSourceBlockId;
			addPhiElementsToPhiCongruenceClasses(phi_iter->first, controlFlowGraph->nodes->operator[](blockId));
	}*/
	ControlFlowGraphPtr controlFlowGraph = livenessAnalyzer->getControlFlowGraph();
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		for(std::vector<InstructionPtr>::iterator inst_iter = currentNode->instructions->begin() ; inst_iter != currentNode->instructions->end() ; ++inst_iter){
			InstructionPtr currentInstruction = *inst_iter;
			if(currentInstruction->type == PHI_FUNCTION){
				addPhiElementsToPhiCongruenceClasses(std::dynamic_pointer_cast<Phi_Function>(currentInstruction)/*, currentNode*/);
			}
			else{
				break;
			}
		}
	}
}

void InterferenceBreaker::initializePhiInterferenceGraph(){
	std::set<Variable> handledVariables;
	for(std::map<Phi_FunctionPtr, std::vector<Variable>>::iterator phi_iter = phiFunctionToVariablesMap.begin() ; phi_iter != phiFunctionToVariablesMap.end() ; ++phi_iter){
		for(std::vector<Variable>::iterator v_iter = phi_iter->second.begin() ; v_iter != phi_iter->second.end() ; ++v_iter){
			if(handledVariables.find(*v_iter) == handledVariables.end()){
				handledVariables.insert(*v_iter);
				std::map<Variable, std::set<Variable>>::iterator interferingSet_iter = livenessAnalyzer->interferenceGraph.find(*v_iter);
				if(interferingSet_iter != livenessAnalyzer->interferenceGraph.end()){
					for(std::set<Variable>::iterator intVar_iter = interferingSet_iter->second.begin() ; intVar_iter != interferingSet_iter->second.end() ; ++intVar_iter){
						if((*v_iter).name.compare((*intVar_iter).name) == 0){
							phiInterferenceMap[*v_iter].insert(*intVar_iter);
						}
					}
				}
			}
		}
	}
}

void InterferenceBreaker::addPhiElementsToPhiCongruenceClasses(const Phi_FunctionPtr &phiFunction/*, Node* containingNode*/){
	//phiFunctionsNodeMap[phiFunction] = containingNode;
	std::vector<Variable> phiVariables;
	constructPhiVariables(phiFunction, phiVariables);
	phiFunctionToVariablesMap[phiFunction] = phiVariables;
	Variable var(phiFunction->target->getName(), phiFunction->target->subscript);
	phiCongruenceClassMap[var].insert(var);
	////phiCongruenceClassInterferenceMap[var] = livenessAnalyzer->interferenceGraph.operator[](var);
	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		Variable var((*arg_iter)->getName(), (*arg_iter)->subscript);
		phiCongruenceClassMap[var].insert(var);
		//phiCongruenceClassInterferenceMap[var] = livenessAnalyzer->interferenceGraph.operator[](var);
	}

	//addElementsToPhiCongruenceClasses(phiFunction->getDefinedElements());
	//addElementsToPhiCongruenceClasses(phiFunction->getUsedElements());
}

//void InterferenceBreaker::addElementsToPhiCongruenceClasses(std::vector<Expression*>* elements){
//	for(std::vector<Expression*>::iterator element_iter = elements->begin() ; element_iter != elements->end() ; ++element_iter){
//		Expression* element = *element_iter;
//		Variable elementVar(element->getName(), element->subscript);
//		phiCongruenceClassMap[elementVar].insert(elementVar);
//		//phiCongruenceClassInterferenceMap[elementVar] = livenessAnalyzer->interferenceGraph.operator[](elementVar);
//	}
//	delete elements;
//}


void InterferenceBreaker::constructPhiVariables(const Phi_FunctionPtr &phiFunction, std::vector<Variable> &phiVariables_out){
	Variable var(phiFunction->target->getName(), phiFunction->target->subscript, phiFunction->target->phiSourceBlockId);
	phiVariables_out.push_back(var);
	std::set<Variable> phiVariablesSet;
	phiVariablesSet.insert(var);
	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		ExpressionPtr phiArgument = *arg_iter;
		Variable currentVatiable(phiArgument->getName(), phiArgument->subscript, phiArgument->phiSourceBlockId);
		std::pair<std::set<Variable>::iterator,bool> ret = phiVariablesSet.insert(currentVatiable);
		if(ret.second)
			phiVariables_out.push_back(currentVatiable);
	}
}

void InterferenceBreaker::clearPhiVariablesUnresolvedNeighborMap(std::vector<Variable> &phiVariables){
	for(std::vector<Variable>::iterator phiVar_iter = phiVariables.begin() ; phiVar_iter != phiVariables.end() ; ++phiVar_iter){
		unresolvedNeighborMap.erase(*phiVar_iter);
	}
}

bool InterferenceBreaker::isPhiTarget(const Phi_FunctionPtr &phiFunction, const Variable &phiVariable){
	return phiFunction->target->getName().compare(phiVariable.name) == 0 && phiFunction->target->subscript == phiVariable.subscript;
}

//bool InterferenceBreaker::isPhiFunctionForMemoryVariables(Phi_Function* phiFunction){
//	if(phiFunction->target->type == POINTER)
//		return true;
//	for(std::vector<Expression*>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
//		if((*arg_iter)->type == POINTER)
//			return true;
//	}
//	return false;
//}

bool InterferenceBreaker::doPhiCongruenceClassesInterfere(const Variable &var1, const Variable &var2){
	/*std::set<Variable>* phiClassVar1 = &(phiCongruenceClassMap[*var1]);
	std::set<Variable>* interferenceClassVar2 = &(phiCongruenceClassInterferenceMap[*var2]);
	std::vector<Variable> commonElements(std::max(phiClassVar1->size(), interferenceClassVar2->size()));
	return commonElements.begin() != std::set_intersection(phiClassVar1->begin(), phiClassVar1->end(), interferenceClassVar2->begin(), interferenceClassVar2->end(), commonElements.begin());*/
	//msg("-------\nphiCongruenceClassMap[%s_%d].size() = %d\n", var1->name.c_str(), var1->subscript, phiCongruenceClassMap[*var1].size());
	//msg("phiCongruenceClassMap[%s_%d].size() = %d\n", var2->name.c_str(), var2->subscript, phiCongruenceClassMap[*var2].size());
	for(std::set<Variable>::iterator it1 = phiCongruenceClassMap[var1].begin() ; it1 != phiCongruenceClassMap[var1].end() ; ++it1){
		for(std::set<Variable>::iterator it2 = phiCongruenceClassMap[var2].begin() ; it2 != phiCongruenceClassMap[var2].end() ; ++it2){
			//Variable x1 = *it1, x2 = *it2;
			if(livenessAnalyzer->doInterfere(*it1, *it2))
				return true;
		}
	}
	return false;
}

bool InterferenceBreaker::doesHaveInterferingCongruenceClasses(const Phi_FunctionPtr &phiFunction, std::vector<Variable> &phiVariables){
	std::set<Variable> combinedCongruenceClass;
	for(std::vector<Variable>::iterator v_iter = phiVariables.begin() ; v_iter != phiVariables.end() ; ++v_iter){
		combinedCongruenceClass.insert(phiCongruenceClassMap[*v_iter].begin(), phiCongruenceClassMap[*v_iter].end());
	}
	std::set<Variable>::iterator v_iter = combinedCongruenceClass.begin();
	while(v_iter != combinedCongruenceClass.end()){
		std::map<Variable, std::set<Variable>>::iterator interferingVar_it = phiInterferenceMap.find(*v_iter);
		if(interferingVar_it != phiInterferenceMap.end()){
			std::set<Variable>* interferingSet = &(interferingVar_it->second);
			std::vector<Variable> commonElements(std::min(combinedCongruenceClass.size(), interferingSet->size()));
			if(std::set_intersection(interferingSet->begin(), interferingSet->end(), combinedCongruenceClass.begin(), combinedCongruenceClass.end(), commonElements.begin())
				!= commonElements.begin()){
					return true;
			}
		}
		v_iter = combinedCongruenceClass.erase(v_iter);
	}
	return false;
}

void InterferenceBreaker::breakInterference(const Phi_FunctionPtr &phiFunction, Variable &var1, Variable &var2){
	bool intersectionLive1AndCongruenceClass2Empty = isPhiTarget(phiFunction, var1)
		? livenessAnalyzer->isIntersectionWithLiveInEmpty(var1.phiSourceBlockId, phiCongruenceClassMap[var2])
		: livenessAnalyzer->isIntersectionWithLiveOutEmpty(var1.phiSourceBlockId, phiCongruenceClassMap[var2]);
	bool intersectionLive2AndCongruenceClass1Empty = isPhiTarget(phiFunction, var2)
		? livenessAnalyzer->isIntersectionWithLiveInEmpty(var2.phiSourceBlockId, phiCongruenceClassMap[var1])
		: livenessAnalyzer->isIntersectionWithLiveOutEmpty(var2.phiSourceBlockId, phiCongruenceClassMap[var1]);

	//msg("\n%s is a Phi %s\n", var1.toString().c_str(), (isPhiTarget(phiFunction, var1) ? "target" : "source"));
	//msg("%s is a Phi %s\n", var2.toString().c_str(), (isPhiTarget(phiFunction, var2) ? "target" : "source"));

	//msg("CongruenceClass(%s) = {", var1.toString().c_str());
	//for(std::set<Variable>::iterator it = phiCongruenceClassMap[var1].begin() ; it != phiCongruenceClassMap[var1].end() ; ++it)
	//	msg("%s, ", it->toString().c_str());
	//msg("}\n");

	//msg("CongruenceClass(%s) = {", var2.toString().c_str());
	//for(std::set<Variable>::iterator it = phiCongruenceClassMap[var2].begin() ; it != phiCongruenceClassMap[var2].end() ; ++it)
	//	msg("%s, ", it->toString().c_str());
	//msg("}\n");

	//msg("Live(%d) ^ CongruenceClass(%s) is %s Empty\n", var1.phiSourceBlockId, var2.toString().c_str(), (intersectionLive1AndCongruenceClass2Empty ? "" : "NOT"));
	//msg("Live(%d) ^ CongruenceClass(%s) is %s Empty\n", var2.phiSourceBlockId, var1.toString().c_str(), (intersectionLive2AndCongruenceClass1Empty ? "" : "NOT"));

	if(!intersectionLive1AndCongruenceClass2Empty)
		candidateResourceSet.insert(var2);
	if(!intersectionLive2AndCongruenceClass1Empty)
		candidateResourceSet.insert(var1);
	if(intersectionLive1AndCongruenceClass2Empty && intersectionLive2AndCongruenceClass1Empty){
		var1.resolved = false;
		var2.resolved = false;
		unresolvedNeighborMap[var1].insert(var2);
		unresolvedNeighborMap[var2].insert(var1);
	}
}

void InterferenceBreaker::handleUnresolvedResources(){
	std::set<Variable> handledResources;
	Variable unresolvedVariable;
	while(getLargestUnresolvedResourceIterator(handledResources, unresolvedVariable)){
		markVariableAsResolved(unresolvedVariable);
		handledResources.insert(unresolvedVariable);
		candidateResourceSet.insert(unresolvedVariable);
		//unresolvedVariable = getLargestUnresolvedResourceIterator(&handledResources);
	}
	removeResourcesWithResolvedNeighboursFromCandidateSet();
}

bool InterferenceBreaker::getLargestUnresolvedResourceIterator(std::set<Variable> &handledResources, Variable &unresolvedVariable_out){
	unsigned int largestSize = 0;
	bool result = false;
	for(std::map<Variable, std::set<Variable>>::iterator unresolved_iter = unresolvedNeighborMap.begin() ; unresolved_iter != unresolvedNeighborMap.end() ; ++unresolved_iter){
		Variable unresolvedVariable = unresolved_iter->first;
		//if(handledResources.find(unresolvedVariable) == handledResources.end() && unresolvedNeighborMap[unresolvedVariable].size() > largestSize && hasUnresolvedNeighbor(unresolvedVariable)){		
		if(handledResources.find(unresolvedVariable) == handledResources.end() && unresolved_iter->second.size() > largestSize && hasUnresolvedNeighbor(unresolvedVariable)){
			unresolvedVariable_out = unresolvedVariable;
			//result = (Variable*)(&(unresolved_iter->first));
			result = true;
			largestSize = unresolved_iter->second.size();
			//largestSize = unresolvedNeighborMap[unresolvedVariable].size();
		}
	}
	return result;
}

bool InterferenceBreaker::hasUnresolvedNeighbor(const Variable &variable){
	for(std::set<Variable>::iterator neighbor_iter = unresolvedNeighborMap[variable].begin() ; neighbor_iter != unresolvedNeighborMap[variable].end() ; ++neighbor_iter){
		if(!(neighbor_iter->resolved))
			return true;
	}
	return false;
}

void InterferenceBreaker::markVariableAsResolved(Variable &variable){
	variable.resolved = true;
	for(std::map<Variable, std::set<Variable>>::iterator unresolved_iter = unresolvedNeighborMap.begin() ; unresolved_iter != unresolvedNeighborMap.end() ; ++unresolved_iter){
		for(std::set<Variable>::iterator unresolvedSetMember_iter = unresolved_iter->second.begin() ; unresolvedSetMember_iter != unresolved_iter->second.end() ; ++unresolvedSetMember_iter){
			//Variable currentVariable = *unresolvedSetMember_iter;//(Variable*)(&(*unresolvedSet_iter));
			if(variable.isEqual(*unresolvedSetMember_iter))
				((Variable*)(&(*unresolvedSetMember_iter)))->resolved = true;
		}
	}
}

void InterferenceBreaker::removeResourcesWithResolvedNeighboursFromCandidateSet(){
	for(std::map<Variable, std::set<Variable>>::iterator unresolved_iter = unresolvedNeighborMap.begin() ; unresolved_iter != unresolvedNeighborMap.end() ; ++unresolved_iter){
		Variable var = unresolved_iter->first;
		if(!hasUnresolvedNeighbor(var) && candidateResourceSet.find(var) != candidateResourceSet.end()){
			candidateResourceSet.erase(var);
		}
	}
}

void InterferenceBreaker::insertCopyStatementsForCandidateResources(Phi_FunctionPtr &phiFunction){
	for(std::set<Variable>::iterator candidate_iter = candidateResourceSet.begin() ; candidate_iter != candidateResourceSet.end() ; ++candidate_iter){
		Variable candidate = *candidate_iter;
		if(isPhiTarget(phiFunction, candidate))
			insertCopyForPhiTarget(phiFunction);
		else
			insertCopyForPhiSource(phiFunction, candidate);
	}
}

void InterferenceBreaker::insertCopyForPhiSource(Phi_FunctionPtr &phiFunction, const Variable &phiSource){
	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		if(phiSource.doesRepresentSameVariable(*arg_iter)){
			ExpressionPtr oldPhiArgument = *arg_iter;
			LocalVariablePtr newPhiArgument = getNewLocalVariableFromExpression(oldPhiArgument);

			AssignmentPtr copyStatement = std::make_shared<Assignment>(newPhiArgument, oldPhiArgument);
			insertInstructionAtEndOfBlock(copyStatement, oldPhiArgument->phiSourceBlockId);
			*arg_iter = newPhiArgument->deepcopy();
			(*arg_iter)->phiSourceBlockId = oldPhiArgument->phiSourceBlockId;//should be in constructor

			livenessAnalyzer->updateDefinitionsMap(newPhiArgument, copyStatement);
			livenessAnalyzer->deleteInstructionFromUsesMap(oldPhiArgument, phiFunction); // think about memory expressions
			livenessAnalyzer->addInstructionToUsesMap(newPhiArgument, phiFunction);

			Variable newPhiArgumentVariable(newPhiArgument->getName(), newPhiArgument->subscript, newPhiArgument->phiSourceBlockId);
			phiCongruenceClassMap[newPhiArgumentVariable].insert(newPhiArgumentVariable);

			Variable oldPhiArgumentVariable(oldPhiArgument->getName(), oldPhiArgument->subscript, oldPhiArgument->phiSourceBlockId);
			livenessAnalyzer->addToLiveOut(newPhiArgumentVariable);

			if(canRemoveOldPhiSourceFromLiveOut(oldPhiArgumentVariable))
				livenessAnalyzer->removeFromLiveOut(oldPhiArgumentVariable);

			livenessAnalyzer->addInterferencesWithLiveOut(newPhiArgumentVariable);
		}
	}
}

void InterferenceBreaker::insertCopyForPhiTarget(Phi_FunctionPtr &phiFunction){
	ExpressionPtr oldPhiTarget = phiFunction->target;
	LocalVariablePtr newPhiTarget = getNewLocalVariableFromExpression(oldPhiTarget);

	AssignmentPtr copyStatement = std::make_shared<Assignment>(oldPhiTarget, newPhiTarget);
	insertInstructionAtBiginingOfBlock(copyStatement, oldPhiTarget->phiSourceBlockId);
	phiFunction->target = newPhiTarget->deepcopy();
	
	livenessAnalyzer->updateDefinitionsMap(newPhiTarget, phiFunction);
	livenessAnalyzer->updateDefinitionsMap(oldPhiTarget, copyStatement);
	livenessAnalyzer->addInstructionToUsesMap(newPhiTarget, copyStatement); // think about memory expressions

	Variable newPhiTargetVariable(newPhiTarget->getName(), newPhiTarget->subscript, newPhiTarget->phiSourceBlockId);
	phiCongruenceClassMap[newPhiTargetVariable].insert(newPhiTargetVariable);

	Variable oldPhiTargetVariable(oldPhiTarget->getName(), oldPhiTarget->subscript, oldPhiTarget->phiSourceBlockId);
	livenessAnalyzer->removeFromLiveIn(oldPhiTargetVariable);
	livenessAnalyzer->addToLiveIn(newPhiTargetVariable);

	livenessAnalyzer->addInterferencesWithLiveIn(newPhiTargetVariable);
}

void InterferenceBreaker::insertInstructionAtBiginingOfBlock(InstructionPtr instruction, int basicBlockId){
	ControlFlowGraphPtr controlFlowGraph = livenessAnalyzer->getControlFlowGraph();
	InstructionVectorPtr blockInstructions = controlFlowGraph->nodes->at(basicBlockId)->instructions;
	for(std::vector<InstructionPtr>::iterator inst_iter = blockInstructions->begin() ; inst_iter != blockInstructions->end() ; ++inst_iter){
		if((*inst_iter)->type != PHI_FUNCTION){
			blockInstructions->insert(inst_iter, instruction);
			break;
		}
	}
}

void InterferenceBreaker::insertInstructionAtEndOfBlock(InstructionPtr instruction, int basicBlockId){
	ControlFlowGraphPtr controlFlowGraph = livenessAnalyzer->getControlFlowGraph();
	InstructionVectorPtr blockInstructions = (*(controlFlowGraph->nodes))[basicBlockId]->instructions;
	std::vector<InstructionPtr>::iterator position = blockInstructions->empty() ? blockInstructions->begin() :
													 blockInstructions->back()->type == CONDITIONAL_JUMP ? blockInstructions->end() - 1 :
													 blockInstructions->end();
	blockInstructions->insert(position, instruction);
}

//void InterferenceBreaker::updateDefiningInstruction(std::string name, int subscript, Instruction* definingInstruction){
//	//livenessAnalyzer->de
//}

LocalVariablePtr InterferenceBreaker::getNewLocalVariableFromExpression(const ExpressionPtr &exp){
	LocalVariablePtr locVar = std::make_shared<LocalVariable>(exp->getName() + "_copy");
	locVar->subscript = copySubscript++;
	locVar->phiSourceBlockId = exp->phiSourceBlockId;
	return locVar;
}

bool InterferenceBreaker::canRemoveOldPhiSourceFromLiveOut(const Variable &oldPhiResourceVariable){
	intSetPtr successorBlocks = livenessAnalyzer->getControlFlowGraph()->getSuccessors(oldPhiResourceVariable.phiSourceBlockId);
	for(std::set<int>::iterator succ_iter = successorBlocks->begin() ; succ_iter != successorBlocks->end() ; ++succ_iter){
		int successorBlockId = *succ_iter;
		if(livenessAnalyzer->isLiveAtBlockEntrance(oldPhiResourceVariable, successorBlockId)){
			return false;
		}
		else{
			ControlFlowGraphPtr controlFlowGraph = livenessAnalyzer->getControlFlowGraph();
			InstructionVectorPtr blockInstructions = controlFlowGraph->nodes->at(successorBlockId)->instructions;
			for(std::vector<InstructionPtr>::iterator inst_iter = blockInstructions->begin() ; inst_iter != blockInstructions->end() ; ++inst_iter){
				InstructionPtr currentInstruction = *inst_iter;
				if(currentInstruction->type == PHI_FUNCTION){
					Phi_FunctionPtr currentPhiFunction = std::dynamic_pointer_cast<Phi_Function>(currentInstruction);
					for(std::vector<ExpressionPtr>::iterator arg_iter = currentPhiFunction->arguments->begin() ; arg_iter != currentPhiFunction->arguments->end() ; ++arg_iter){
						ExpressionPtr argument = *arg_iter;
						if(oldPhiResourceVariable.doesRepresentSameVariable(argument) && argument->phiSourceBlockId == oldPhiResourceVariable.phiSourceBlockId)
							return false;
					}
				}
			}
		}
	}
	return true;
}

void InterferenceBreaker::updatePhiCongruenceClassMap(const Phi_FunctionPtr &phiFunction){
	std::set<Variable> combinedPhiCongruenceClass/*, combinedPhiCongruenceClassInterference*/;
	Variable phiTargetVariable(phiFunction->target->getName(), phiFunction->target->subscript, phiFunction->target->phiSourceBlockId);
	combinedPhiCongruenceClass.insert(phiCongruenceClassMap[phiTargetVariable].begin(), phiCongruenceClassMap[phiTargetVariable].end());
	//combinedPhiCongruenceClassInterference.insert(phiCongruenceClassInterferenceMap[phiTargetVariable].begin(), phiCongruenceClassInterferenceMap[phiTargetVariable].end());

	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		ExpressionPtr argument = *arg_iter;
		Variable phiSourceVariable(argument->getName(), argument->subscript, argument->phiSourceBlockId);
		combinedPhiCongruenceClass.insert(phiCongruenceClassMap[phiSourceVariable].begin(), phiCongruenceClassMap[phiSourceVariable].end());
		//combinedPhiCongruenceClassInterference.insert(phiCongruenceClassInterferenceMap[phiSourceVariable].begin(), phiCongruenceClassInterferenceMap[phiSourceVariable].end());
	}

	for(std::set<Variable>::iterator var_iter = combinedPhiCongruenceClass.begin() ; var_iter != combinedPhiCongruenceClass.end() ; ++var_iter){
		phiCongruenceClassMap[*var_iter] = combinedPhiCongruenceClass;
		//phiCongruenceClassInterferenceMap[*var_iter] = combinedPhiCongruenceClassInterference;
	}
}

void InterferenceBreaker::nullifyPhiCongruenceClassesWithSingletonResources(){
	for(std::map<Variable, std::set<Variable>>::iterator it = phiCongruenceClassMap.begin() ; it != phiCongruenceClassMap.end() ; ++it){
		if(it->second.size() == 1)
			it->second.clear();
	}
}


/*void InterferenceBreaker::eliminatePhiFunctions(){
	std::string phiRepresentativeName = "local";
	int phiRepresentativeSbscript = 1;
	bool needNewSubscript = false;
	for(std::map<Variable, std::set<Variable>>::iterator x_iter = phiCongruenceClassMap.begin() ; x_iter = phiCongruenceClassMap.end() ; ++x_iter){
		if(needNewSubscript){
			i++;
			needNewSubscript = false;
		}
		Variable x = *x_iter;
		for(std::set<Variable>::iterator xi_iter = x_iter->second.begin() ; xi_iter != x_iter->second.begin() ; ++xi_iter){
			Variable xi = *xi_iter;
			if(livenessAnalyzer->isVariableDefined(xi.name, xi.subscript)){
				Instruction* definingInstruction = NULL;
			}
			if(!livenessAnalyzer->isVariableDead(xi.name, xi.subscript)){
				
			}
		}
	}
}*/
