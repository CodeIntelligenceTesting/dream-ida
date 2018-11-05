////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "LivenessAnalysis.h"

#include <algorithm>
#include <utility>
#include <iterator>

#include <boost/lexical_cast.hpp>

#include "../DataStructures/Variable.h"
#include "../../Shared/InstructionUtilities.h"



LivenessAnalysis::LivenessAnalysis(ControlFlowGraphPtr _controlFlowGraph,
									definitionsMapPtr _definitionsMap,
									//std::map<std::string, std::map<int, Instruction*>>* _flagDefinitionsMap,
									usesMapPtr _usesMap)
									: DataFlowAlgorithm(_controlFlowGraph, _definitionsMap, std::shared_ptr<std::map<std::string, std::map<int, InstructionPtr>>>(), _usesMap)
{
	memorySubscript = 0;
}

LivenessAnalysis::~LivenessAnalysis(void)
{
}

void LivenessAnalysis::applyAlgorithm(){
	addPhiTargetsToLiveIn();
	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator defName_iter = definitionsMap->begin() ; defName_iter != definitionsMap->end() ; ++defName_iter){
		for(std::map<int, InstructionPtr>::iterator defSubscript_iter = defName_iter->second.begin() ; defSubscript_iter != defName_iter->second.end() ; ++defSubscript_iter){
			handledBasicBlocks.clear();
			Variable variable(defName_iter->first, defSubscript_iter->first);
			computeVariableLiveness(variable);
		}
	}
	Variable variable("eax", 0);
	computeVariableLiveness(variable);
	/*for(std::map<int, std::set<Variable>>::iterator iter = liveIn.begin() ; iter != liveIn.end() ; ++iter){
		int nodeId = iter->first;
		msg("LiveIn(%d): ", nodeId);
		for(std::set<Variable>::iterator vit = liveIn[nodeId].begin() ; vit != liveIn[nodeId].end() ; ++vit){
			msg("%s, ", (*vit).toString().c_str());
		}
		msg("\n");
	}
	for(std::map<int, std::set<Variable>>::iterator iter = liveOut.begin() ; iter != liveOut.end() ; ++iter){
		int nodeId = iter->first;
		msg("liveOut(%d): ", nodeId);
		for(std::set<Variable>::iterator vit = liveOut[nodeId].begin() ; vit != liveOut[nodeId].end() ; ++vit){
			msg("%s, ", (*vit).toString().c_str());
		}
		msg("\n");
	}
	for(std::map<Variable, std::set<Variable>>::iterator iter = interferenceGraph.begin() ; iter != interferenceGraph.end() ; ++iter){
		msg("interfer(%s) = ", iter->first.toString().c_str());
		for(std::set<Variable>::iterator vit = iter->second.begin() ; vit != iter->second.end() ; ++vit){
			msg("%s, ", (*vit).toString().c_str());
		}
		msg("\n");
	}*/
}

void LivenessAnalysis::computeVariableLiveness(const Variable &variable){
	//msg("computeVariableLiveness(%s)\n", variable->toString().c_str());
	std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(variable.name, variable.subscript);//&((*usesMap)[variable->name][variable->subscript]);
	for(std::vector<InstructionPtr>::iterator usingInstruction_iter = usingInstructions->begin() ; usingInstruction_iter != usingInstructions->end() ; ++usingInstruction_iter){
		InstructionPtr usingInstruction = *usingInstruction_iter;
		if(usingInstruction->type == PHI_FUNCTION){
			Phi_FunctionPtr usingPhiFunction = std::dynamic_pointer_cast<Phi_Function>(usingInstruction);
			makePhiArgumentsLiveOutAtBlock(usingPhiFunction, variable);
			/*Phi_Function* usingPhiFunction = (Phi_Function*)usingInstruction;
			std::set<int>* PhiArgumentDefiningNodes = &(usingPhiFunction->definingNode[variable->name][variable->subscript]);
			for(std::set<int>::iterator phiArgDefNode_iter = PhiArgumentDefiningNodes->begin() ; phiArgDefNode_iter != PhiArgumentDefiningNodes->end() ; ++phiArgDefNode_iter){
				makeLiveOutAtBlock(*phiArgDefNode_iter, variable);
			}*/
		}
		else{
			makeLiveInAtInstruction(usingInstruction, variable);
		}
	}
}

void LivenessAnalysis::addPhiTargetsToLiveIn(){
	for(std::map<int, NodePtr>::iterator bb_iter = controlFlowGraph->nodes->begin() ; bb_iter != controlFlowGraph->nodes->end() ; ++bb_iter){
		addPhiTargetsToBasicBlockLiveIn(bb_iter->first, bb_iter->second);
	}
}

void LivenessAnalysis::addPhiTargetsToBasicBlockLiveIn(int basicBlockId, const NodePtr &basicBlock){
	for(std::vector<InstructionPtr>::iterator inst_iter = basicBlock->instructions->begin() ; inst_iter != basicBlock->instructions->end() ; ++inst_iter){
		InstructionPtr currentInstruction = *inst_iter;
		if(currentInstruction->type != PHI_FUNCTION)
			return;

		addVariablesToBasicBlockLiveIn(basicBlockId, getDefinedElements(currentInstruction)/*currentInstruction->getDefinedElements()*/);
	}
}

void LivenessAnalysis::addVariablesToBasicBlockLiveIn(int basicBlockId, ExpressionVectorPtr variables){
	for(std::vector<ExpressionPtr>::iterator variable_iter = variables->begin() ; variable_iter != variables->end() ; ++variable_iter){
		ExpressionPtr variableExpression = *variable_iter;
		liveIn[basicBlockId].insert(Variable(variableExpression->getName(), variableExpression->subscript));
	}
}

bool LivenessAnalysis::doInterfere(const Variable &var1, const Variable &var2){
	std::set<Variable>* interferingVariables = &(interferenceGraph[var1]);
	return interferingVariables->find(var2) != interferingVariables->end();
}

bool LivenessAnalysis::isBlockHandled(int basicBlockId){
	return handledBasicBlocks.find(basicBlockId) != handledBasicBlocks.end();
}

bool LivenessAnalysis::areNotAliases(const Variable &v1, const Variable &v2){
	return relatedRegistersMap[v1.name].find(v2.name) != relatedRegistersMap[v1.name].end()
			//? (*definitionsMap)[v1.name][v1.subscript] != (*definitionsMap)[v2.name][v2.subscript]
			? definitionsMap->operator[](v1.name).operator[](v1.subscript) != definitionsMap->operator[](v2.name).operator[](v2.subscript)
			: (v1.name.compare(v2.name) != 0 || v1.subscript != v2.subscript);
}

void LivenessAnalysis::makeLiveInAtInstruction(const InstructionPtr &instruction, const Variable &variable){
	//msg("makeLiveInAtInstruction(%s, %s)\n", instruction->getInstructionString().c_str(), variable->toString().c_str());
	std::pair<int, int> ids;
	controlFlowGraph->computeInstructionIds(instruction, ids);
	int basicBlockId = ids.first;
	int instructionId = ids.second;
	//msg("%d, %d\n", basicBlockId, instructionId);
	if(basicBlockId != -1){
		if(instructionId == 0){
			liveIn[basicBlockId].insert(variable);
			intSetPtr predecessorBlocks = controlFlowGraph->getPredecessors(basicBlockId);
			for(std::set<int>::iterator predecessorBlock_iter = predecessorBlocks->begin() ; predecessorBlock_iter != predecessorBlocks->end() ; ++predecessorBlock_iter){
				makeLiveOutAtBlock(*predecessorBlock_iter, variable);
			}
		}
		else{
			InstructionVectorPtr blockInstructions = controlFlowGraph->nodes->at(basicBlockId)->instructions;
			InstructionPtr precedingInstruction = blockInstructions->operator[](instructionId - 1);
			makeLiveOutAtInstruction(precedingInstruction, variable);
		}
	}
}

void LivenessAnalysis::makeLiveOutAtInstruction(const InstructionPtr &instruction, const Variable &variable){
	//msg("makeLiveOutAtInstruction(%s, %s)\n", instruction->getInstructionString().c_str(), variable->toString().c_str());
	bool isVariableDefinedByInstruction = false;
	ExpressionVectorPtr definedElements = getDefinedElements(instruction);//instruction->getDefinedElements();numFunctionCalls++;
	if(!definedElements->empty()){
		for(std::vector<ExpressionPtr>::iterator definedElement_iter = definedElements->begin() ; definedElement_iter != definedElements->end() ; ++definedElement_iter){
			Variable definedVariable((*definedElement_iter)->getName(), (*definedElement_iter)->subscript);
			if(areNotAliases(variable, definedVariable)){
				addToInterferenceGraph(variable, definedVariable);
			}
			else{
				isVariableDefinedByInstruction = true;
			}
		}
	}
	if(!isVariableDefinedByInstruction){
		makeLiveInAtInstruction(instruction, variable);
	}
}

void LivenessAnalysis::makePhiArgumentsLiveOutAtBlock(const Phi_FunctionPtr &phiFunction, const Variable &variable){
	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		if(variable.doesRepresentSameVariable(*arg_iter)){
			makeLiveOutAtBlock((*arg_iter)->phiSourceBlockId, variable);
		}
	}
}

void LivenessAnalysis::makeLiveOutAtBlock(int basicBlockId, const Variable &variable){
	//msg("makeLiveOutAtBlock(%d, %s)\n", basicBlockId, variable->toString().c_str());
	liveOut[basicBlockId].insert(variable);
	//msg("liveOut[%d].insert(%s)\n", basicBlockId, variable->toString().c_str());
	if(!isBlockHandled(basicBlockId)){
		handledBasicBlocks.insert(basicBlockId);
		InstructionVectorPtr instructions = controlFlowGraph->nodes->at(basicBlockId)->instructions;
		if(!instructions->empty()){
			makeLiveOutAtInstruction(instructions->back(), variable);
		}
	}
}

void LivenessAnalysis::addToInterferenceGraph(const Variable &v1, const Variable &v2){
	interferenceGraph[v1].insert(v2);
	interferenceGraph[v2].insert(v1);
}


bool LivenessAnalysis::isIntersectionWithLiveOutEmpty(int basicBlockId, std::set<Variable> &variables){
	/*msg("LiveOut(%d) = {", basicBlockId);
	for(std::set<Variable>::iterator it = liveOut[basicBlockId].begin() ; it != liveOut[basicBlockId].end() ; ++it)
		msg("%s, ", it->toString().c_str());
	msg("}\n");*/
	return isIntersectionEmpty(liveOut[basicBlockId], variables);
}

bool LivenessAnalysis::isIntersectionWithLiveInEmpty(int basicBlockId, std::set<Variable> &variables){
	return isIntersectionEmpty(liveIn[basicBlockId], variables);
}

bool LivenessAnalysis::isIntersectionEmpty(std::set<Variable> &set1, std::set<Variable> &set2){
	std::vector<Variable> commonElements(std::min(set1.size(), set2.size()));
	return std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), commonElements.begin()) == commonElements.begin();
}

void LivenessAnalysis::addToLiveIn(const Variable &varToAdd){
	addToSet(liveIn[varToAdd.phiSourceBlockId], varToAdd);
}

void LivenessAnalysis::removeFromLiveIn(const Variable &varToRemove){
	removeFromSet(liveIn[varToRemove.phiSourceBlockId], varToRemove);
}

void LivenessAnalysis::addToLiveOut(const Variable &varToAdd){
	addToSet(liveOut[varToAdd.phiSourceBlockId], varToAdd);
}

void LivenessAnalysis::removeFromLiveOut(const Variable &varToRemove){
	removeFromSet(liveOut[varToRemove.phiSourceBlockId], varToRemove);
}

void LivenessAnalysis::addToSet(std::set<Variable> &targetSet, const Variable &varToAdd){
	targetSet.insert(varToAdd);
}

void LivenessAnalysis::removeFromSet(std::set<Variable> &targetSet, const Variable &varToRemove){
	targetSet.erase(varToRemove);
}

void LivenessAnalysis::addInterferencesWithLiveIn(const Variable &interferingVariable){
	addInterferencesWithSet(liveIn[interferingVariable.phiSourceBlockId], interferingVariable);
}

void LivenessAnalysis::addInterferencesWithLiveOut(const Variable &interferingVariable){
	addInterferencesWithSet(liveOut[interferingVariable.phiSourceBlockId], interferingVariable);
}

void LivenessAnalysis::addInterferencesWithSet(std::set<Variable> &targetSet, const Variable &interferingVariable){
	for(std::set<Variable>::iterator var_iter = targetSet.begin() ; var_iter != targetSet.end() ; ++var_iter){
		interferenceGraph[interferingVariable].insert(*var_iter);
		interferenceGraph[*var_iter].insert(interferingVariable);
	}
}

bool LivenessAnalysis::isLiveAtBlockEntrance(const Variable &variable, int basicBlockId){
	return liveIn[basicBlockId].find(variable) != liveIn[basicBlockId].end();
}

bool LivenessAnalysis::isLiveAtBlockExit(const Variable &variable, int basicBlockId){
	return liveOut[basicBlockId].find(variable) != liveOut[basicBlockId].end();
}

ExpressionVectorPtr LivenessAnalysis::getDefinedElements(const InstructionPtr &definingInstruction){
	std::map<InstructionPtr, ExpressionVectorPtr>::iterator defs_iter = instructionToDefinedElementsMap.find(definingInstruction);
	if(defs_iter == instructionToDefinedElementsMap.end()){
		ExpressionVectorPtr definedElements = std::make_shared<std::vector<ExpressionPtr>>();
		definingInstruction->getDefinedElements(*definedElements);
		//std::vector<Expression*>* definedElements = definingInstruction->getDefinedElements();
		instructionToDefinedElementsMap[definingInstruction] = definedElements;
		return definedElements;
	}
	else{
		return defs_iter->second;
	}
}

void LivenessAnalysis::replaceInterferingMamoryLocations(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		InstructionVectorPtr instructions = node_iter->second->instructions;
		for(int instId = 0 ; instId != instructions->size() ; ++instId){
			InstructionPtr currentInstruction = instructions->at(instId);
			if(currentInstruction->type == PHI_FUNCTION){
				Phi_FunctionPtr currentPhiFunction = std::dynamic_pointer_cast<Phi_Function>(currentInstruction);
				if(currentPhiFunction->target->isMemoryVariable() && doPhiResourcesInterfere(currentPhiFunction)){
					replaceVariables(currentPhiFunction->target, getCorrespondingLocalVariable(currentPhiFunction->target));
				}
			}
			else{
				ExpressionVectorPtr definedElements = getDefinedElements(instructions->at(instId))/*instructions->at(instId)->getDefinedElements()*/;
				if(!definedElements->empty()){
					for(std::vector<ExpressionPtr>::iterator def_iter = definedElements->begin() ; def_iter != definedElements->end() ; ++def_iter){
						ExpressionPtr definedVariable = *def_iter;
						if(definedVariable->isMemoryVariable()){
							if(doesInterfereWithOtherVersions(definedVariable) || isInPhiFunctionWithInterferingResources(definedVariable)){
								LocalVariablePtr copyTarget = getCorrespondingLocalVariable(definedVariable);

								definitionsMap->operator[](definedVariable->getName())[definedVariable->subscript].reset();// = NULL;
								definedVariable->subscript = NO_SUBSCRIPT;
								ExpressionPtr copySource = definedVariable->deepcopy();
								InstructionPtr copyStatement = std::make_shared<Assignment>(copyTarget, copySource);

								instructions->insert(instructions->begin() + instId + 1, copyStatement);
								updateDefinitionsMap(copyTarget, copyStatement);
								instId++;
								replaceUses(copySource, copyTarget);
							}
							else if(isInstructionPointerNull(getRelatedPhiFunction(definedVariable))/*getRelatedPhiFunction(definedVariable) == NULL*/){
								removeSubscriptsFromUses(definedVariable);
								definedVariable->subscript = NO_SUBSCRIPT;
							}
						}
					}
				}
			}
		}
	}
}

LocalVariablePtr LivenessAnalysis::getCorrespondingLocalVariable(const ExpressionPtr &exp){
	LocalVariablePtr memMirror = std::make_shared<LocalVariable>("memMirror");
	memMirror->subscript = (memorySubscript++);
	memMirror->phiSourceBlockId = exp->phiSourceBlockId;
	return memMirror;
}

bool LivenessAnalysis::doesInterfereWithOtherVersions(const ExpressionPtr &variable){
	Variable testedVariable(variable->getName(), variable->subscript);
	std::map<Variable, std::set<Variable>>::iterator varIter = interferenceGraph.find(testedVariable);
	if(varIter != interferenceGraph.end()){
		for(std::set<Variable>::iterator interferingVarIter = varIter->second.begin() ; interferingVarIter != varIter->second.end() ; ++interferingVarIter){
			if(testedVariable.name.compare(interferingVarIter->name) == 0){
				return true;
			}
		}
	}
	return false;
}

//void LivenessAnalysis::addCopyStatement(Expression* expToCopy, Instruction* afterThisInstruction){
//	LocalVariable* copyExpression = new LocalVariable("memMirror"/* + boost::lexical_cast<std::string>(memorySubscript++)*/);
//	copyExpression->subscript = expToCopy->subscript;
//}

void LivenessAnalysis::replaceVariables(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable){
	replaceInDefinitionsMap(replacedVariable, replacingVariable);
	replaceInUsesMap(replacedVariable, replacingVariable);
	replaceInLiveMap(replacedVariable, replacingVariable, liveIn);
	replaceInLiveMap(replacedVariable, replacingVariable, liveOut);
	replaceInInterferenceGraph(replacedVariable, replacingVariable);
}

void LivenessAnalysis::replaceUses(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable){
	replaceInUsesMap(replacedVariable, replacingVariable);
	replaceInLiveMap(replacedVariable, replacingVariable, liveIn);
	replaceInLiveMap(replacedVariable, replacingVariable, liveOut);
	replaceInInterferenceGraph(replacedVariable, replacingVariable);
}

void LivenessAnalysis::replaceInDefinitionsMap(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable){
	std::map<std::string, std::map<int, InstructionPtr>>::iterator defName_iter = definitionsMap->find(replacedVariable->getName());
	if(defName_iter != definitionsMap->end()){
		std::map<int, InstructionPtr>::iterator defSubscript_iter = defName_iter->second.find(replacedVariable->subscript);
		if(defSubscript_iter != defName_iter->second.end()){
			InstructionPtr definingInstruction = defSubscript_iter->second;
			defSubscript_iter->second.reset();
			definingInstruction->replaceDefinition(replacedVariable->getName(), replacedVariable->subscript, replacingVariable);
			definitionsMap->operator[](replacingVariable->getName()).insert(std::pair<int, InstructionPtr>(replacingVariable->subscript, definingInstruction));
		}
	}
}

void LivenessAnalysis::replaceInUsesMap(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable){
	std::vector<InstructionPtr>* oldUses = getUsingInstructions(replacedVariable->getName(), replacedVariable->subscript);//&(usesMap->operator[](replacedVariable.getName()).operator[](replacedVariable.subscript));
	std::vector<InstructionPtr>* newUses = getUsingInstructions(replacingVariable->getName(), replacingVariable->subscript);//&(usesMap->operator[](replacingVariable.getName()).operator[](replacingVariable.subscript));
	for(std::vector<InstructionPtr>::iterator oldUse_iter = oldUses->begin() ; oldUse_iter != oldUses->end() ; ++oldUse_iter){
		InstructionPtr usingInstruction = *oldUse_iter;
		usingInstruction->replaceUse(replacedVariable->getName(), replacedVariable->subscript, replacingVariable);
		newUses->push_back(usingInstruction);
	}
	oldUses->clear();
}

void LivenessAnalysis::replaceInLiveMap(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable, std::map<int, std::set<Variable>> &liveMap){
	Variable variableToRemove(replacedVariable->getName(), replacedVariable->subscript);
	std::set<Variable>::iterator removeIter;
	for(std::map<int, std::set<Variable>>::iterator liveIn_iter = liveMap.begin() ; liveIn_iter != liveMap.end() ; ++liveIn_iter){
		removeIter = liveIn_iter->second.find(variableToRemove);
		if(removeIter != liveIn_iter->second.end()){
			liveIn_iter->second.erase(removeIter);
			Variable variableToAdd(replacingVariable->getName(), replacingVariable->subscript);
			liveIn_iter->second.insert(variableToAdd);
		}
	}
}

void LivenessAnalysis::replaceInInterferenceGraph(const ExpressionPtr &replacedVariable, const ExpressionPtr &replacingVariable){
	Variable variableToRemove(replacedVariable->getName(), replacedVariable->subscript);
	std::map<Variable, std::set<Variable>>::iterator removeIter = interferenceGraph.find(variableToRemove);
	if(removeIter != interferenceGraph.end()){
		std::set<Variable> newInterferingSet = removeIter->second;
		interferenceGraph.erase(removeIter);
		Variable variableToAdd(replacingVariable->getName(), replacingVariable->subscript);
		for(std::set<Variable>::iterator replaceIter = newInterferingSet.begin() ; replaceIter != newInterferingSet.end() ; ++replaceIter){
			std::set<Variable>* interferingSet = &(interferenceGraph[*replaceIter]);
			interferingSet->erase(variableToRemove);
			interferingSet->insert(variableToAdd);
		}
		interferenceGraph[variableToAdd] = newInterferingSet;
	}
}

bool LivenessAnalysis::doPhiResourcesInterfere(const Phi_FunctionPtr &phiFunction){
	std::set<Variable> phiResources;
	phiResources.insert(Variable(phiFunction->target->getName(), phiFunction->target->subscript));
	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		phiResources.insert(Variable((*arg_iter)->getName(), (*arg_iter)->subscript));
	}

	for(std::set<Variable>::iterator phiVarIter = phiResources.begin() ; phiVarIter != phiResources.end() ; ++phiVarIter){
		std::map<Variable, std::set<Variable>>::iterator phiInterferenceSetIter = interferenceGraph.find(*phiVarIter);
		if(phiInterferenceSetIter != interferenceGraph.end()){
			std::vector<Variable> intersectionVector(std::min(phiResources.size(), phiInterferenceSetIter->second.size()));
			std::vector<Variable>::iterator it = std::set_intersection(phiResources.begin(),
				phiResources.end(), phiInterferenceSetIter->second.begin(), phiInterferenceSetIter->second.end(), intersectionVector.begin());
			if(it != intersectionVector.begin())
				return true;
		}
	}
	return false;
	/*while(phiResources.size() >= 2){
		Variable testedPhiVariable = *(phiResources.begin());
		for(std::set<Variable>::iterator phiVarIter = phiResources.begin() ; phiVarIter != phiResources.end() ; ++phiVarIter){
			
		}
	}*/
}

bool LivenessAnalysis::isInPhiFunctionWithInterferingResources(const ExpressionPtr &exp){
	Phi_FunctionPtr relatedPhiFunction = getRelatedPhiFunction(exp);
	if(!isInstructionPointerNull(relatedPhiFunction)){
		return doPhiResourcesInterfere(relatedPhiFunction);
	}
	return false;
}

Phi_FunctionPtr LivenessAnalysis::getRelatedPhiFunction(const ExpressionPtr &exp){
	InstructionPtr definingInstruction = getDefiningInstruction(exp->getName(), exp->subscript);
	if(definingInstruction.get() != NULL && definingInstruction->type == PHI_FUNCTION)
		return std::dynamic_pointer_cast<Phi_Function>(definingInstruction);
	std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(exp->getName(), exp->subscript);//&(usesMap->operator[](exp->getName()).operator[](exp->subscript));
	for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
		if((*use_iter)->type == PHI_FUNCTION)
			return std::dynamic_pointer_cast<Phi_Function>(*use_iter);
	}
	return std::shared_ptr<Phi_Function>();
}

void LivenessAnalysis::removeSubscriptsFromUses(ExpressionPtr &exp){
	std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(exp->getName(), exp->subscript);
	std::string variableName = exp->getName();
	int variableSubscript = exp->subscript;
	for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
		std::vector<ExpressionPtr> usedElements;
		(*use_iter)->getUsedElements(usedElements);
		if(!usedElements.empty()){
			for(std::vector<ExpressionPtr>::iterator element_iter = usedElements.begin() ; element_iter != usedElements.end() ; ++element_iter){
				if((*element_iter)->isSameVariable(variableName, variableSubscript)){
					(*element_iter)->subscript = NO_SUBSCRIPT;
				}
			}
		}
	}
}
