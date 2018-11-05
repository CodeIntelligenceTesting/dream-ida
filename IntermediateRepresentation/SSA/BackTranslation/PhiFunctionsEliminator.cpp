////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "PhiFunctionsEliminator.h"

#include "../../../DataFlowAnalysis/DataStructures/Variable.h"
#include "../../ControlFlowGraph.h"
#include "../../../Shared/RelatedRegistersMonitor.h"
#include "../../../Shared/InstructionUtilities.h"

#include "../../../TypeAnalysis/TypeMap.h"
#include "../../../TypeAnalysis/Types/TopType.h"

#include <set>

#include <boost/lexical_cast.hpp>


PhiFunctionsEliminator::PhiFunctionsEliminator(ControlFlowGraphPtr _controlFlowGraph,
						   definitionsMapPtr _definitionsMap,
						   usesMapPtr _usesMap,
						   TypeMapPtr _typeMap,
						   std::map<Variable, std::set<Variable>>* _phiCongruenceClassMap,
						   ExpressionVectorPtr _functionArguments)
{
	controlFlowGraph = _controlFlowGraph;
	definitionsMap = _definitionsMap;
	usesMap = _usesMap;
	typeMap = _typeMap;
	phiCongruenceClassMap = _phiCongruenceClassMap;
	relatedRegistersMonitor = std::make_shared<RelatedRegistersMonitor>(definitionsMap, usesMap);
	phiRepresentativeSubscript = 1;
	functionArguments = _functionArguments;
}


PhiFunctionsEliminator::~PhiFunctionsEliminator(void)
{
}

void PhiFunctionsEliminator::eliminatePhiFunctions(){
	eliminatePhiFunctionsWithMemoryVariables();
	eliminatePhiFunctionsWithSimpleVariables();
	removeZeroSubscripts();
	//std::vector<Variable> simpleVariables;
	//std::set<Variable> handledVariables;

	//for(std::map<Variable, std::set<Variable>>::iterator x_iter = phiCongruenceClassMap->begin() ; x_iter != phiCongruenceClassMap->end() ; ++x_iter){
	//	if(handledVariables.find(x_iter->first) == handledVariables.end()){
	//		Variable currentVariable = x_iter->first;
	//		Instruction* definingInstruction = definitionsMap->operator[](currentVariable.name)[currentVariable.subscript];
	//		bool isPointer = definingInstruction != NULL ? isMemoryVariable(&currentVariable, definingInstruction) : false;
	//		if(isPointer){
	//			handledVariables.insert(currentVariable);//may not be necessary
	//			for(std::set<Variable>::iterator xi_iter = x_iter->second.begin() ; xi_iter != x_iter->second.end() ; ++xi_iter){
	//				Variable xi = *xi_iter;
	//				handledVariables.insert(xi);
	//				replaceMemoryVariable(&xi, definingInstruction);
	//			}
	//		}
	//		else{
	//			simpleVariables.push_back(currentVariable);
	//		}
	//	}

	//	//if(handledVariables.find(x_iter->first) == handledVariables.end()){
	//	//	handledVariables.insert(x_iter->first);
	//	//	
	//	//	Variable x = x_iter->first;
	//	//	for(std::set<Variable>::iterator xi_iter = x_iter->second.begin() ; xi_iter != x_iter->second.end() ; ++xi_iter){
	//	//		Variable xi = *xi_iter;
	//	//		handledVariables.insert(xi);
	//	//		Instruction* definingInstruction = definitionsMap->operator[](xi.name)[xi.subscript];
	//	//		bool isPointer = definingInstruction != NULL ? isMemoryVariable(&xi, definingInstruction) : false;
	//	//		
	//	//		if(isVariableDefined(&xi)){
	//	//			if(definingInstruction->type == PHI_FUNCTION)
	//	//				removeInstruction(definingInstruction);
	//	//			else{
	//	//				//replace potential definitions of instruction
	//	//				//replace potential uses of instruction <NOT necessary>
	//	//				if(isPointer){
	//	//					removeSubscriptOfDefintion(&xi, definingInstruction);
	//	//				}
	//	//				else{
	//	//					LocalVariable renamedVariable(phiRepresentativeName + boost::lexical_cast<std::string>(phiRepresentativeSubscript));
	//	//					Variable* relatedVariable = relatedRegistersMonitor->getDefinedRelatedRegister(xi.name, xi.subscript);
	//	//					needNewSubscript = definingInstruction->replaceDefinition(relatedVariable->name, relatedVariable->subscript, &renamedVariable);
	//	//				}
	//	//			}
	//	//		}
	//	//		std::vector<Variable>* relatedUses = relatedRegistersMonitor->getUsedRelatedRegisters(xi.name, xi.subscript);
	//	//		if(relatedUses != NULL)
	//	//			for(std::vector<Variable>::iterator it = relatedUses->begin() ; it != relatedUses->end() ; ++it){
	//	//				std::vector<Instruction*>* usingInstructions = &(usesMap->operator[](it->name)[it->subscript]);
	//	//				//msg("replace %s_%d in\n", it->name.c_str(), it->subscript);
	//	//				for(std::vector<Instruction*>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
	//	//					Instruction* usingInstruction = *use_iter;
	//	//					//msg("\t%s\n", usingInstruction->getInstructionString().c_str());
	//	//					if(usingInstruction->type != PHI_FUNCTION){
	//	//						//replace potential definitions of instruction <NOT necessary>
	//	//						//replace potential uses of instruction
	//	//						if(isPointer){
	//	//							removeSubscriptOfUse(&(*it), usingInstruction);
	//	//							//msg("removeSubscriptOfUse(%s_%d)\n", it->name.c_str(), it->subscript);
	//	//						}
	//	//						else{
	//	//							LocalVariable renamedVariable(phiRepresentativeName + boost::lexical_cast<std::string>(phiRepresentativeSubscript));
	//	//							needNewSubscript |= usingInstruction->replaceUse(it->name, it->subscript, &renamedVariable);
	//	//						}
	//	//					}
	//	//				}
	//	//			}
	//	//	}
	//	//}
	//}
}

bool PhiFunctionsEliminator::isVariableDefined(const Variable &var){
	return !isInstructionPointerNull(definitionsMap->operator[](var.name).operator[](var.subscript));
}

bool PhiFunctionsEliminator::isVariableUsed(const Variable &var){
	return !usesMap->operator[](var.name).operator[](var.subscript).empty();
}

void PhiFunctionsEliminator::eliminatePhiFunctionsWithMemoryVariables(){
	std::set<Variable> handledVariables;
	std::map<Variable, std::set<Variable>>::iterator x_iter = phiCongruenceClassMap->begin();
	while(x_iter != phiCongruenceClassMap->end()){
		if(handledVariables.find(x_iter->first) == handledVariables.end()){
			Variable currentVariable = x_iter->first;
			//msg("currentVariable %s_%d\n", currentVariable.name.c_str(), currentVariable.subscript);
			if(isMemoryVariable(currentVariable)){
				//msg("isMemoryVariable\n");
				for(std::set<Variable>::iterator xi_iter = x_iter->second.begin() ; xi_iter != x_iter->second.end() ; ++xi_iter){
					Variable xi = *xi_iter;
					std::pair<std::set<Variable>::iterator, bool> ret = handledVariables.insert(xi);
					//msg("replace %s%d ....", xi.name.c_str(), xi.subscript);
					if(ret.second == true)
						replaceMemoryVariable(xi);
					//msg("done\n");
				}
				x_iter = phiCongruenceClassMap->erase(x_iter);
			}
			else{
				++x_iter;
			}
		}
		else{
			x_iter = phiCongruenceClassMap->erase(x_iter);
		}
	}
}

void PhiFunctionsEliminator::replaceMemoryVariable(const Variable &var){
	if(isVariableDefined(var)){
		InstructionPtr definingInstruction = definitionsMap->operator[](var.name).operator[](var.subscript);
		if(definingInstruction->type == PHI_FUNCTION)
			removeInstruction(definingInstruction);
		else
			removeSubscriptOfDefintion(var, definingInstruction);
	}
	std::vector<Variable> relatedUses;
	relatedRegistersMonitor->getUsedRelatedRegisters(var.name, var.subscript, relatedUses);
	if(!relatedUses.empty())
		for(std::vector<Variable>::iterator it = relatedUses.begin() ; it != relatedUses.end() ; ++it){
			std::vector<InstructionPtr>* usingInstructions = &(usesMap->operator[](it->name).operator[](it->subscript));
			for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
				InstructionPtr usingInstruction = *use_iter;
				if(usingInstruction->type != PHI_FUNCTION){
					removeSubscriptOfUse(*it, usingInstruction);
				}
			}
		}
}

void PhiFunctionsEliminator::eliminatePhiFunctionsWithSimpleVariables(){
	//msg("eliminatePhiFunctionsWithSimpleVariables()\n");
	char currentName = 'a';
	int nameLength = 1;
	std::set<Variable> handledVariables;
	bool isNewSubscriptNeeded = false;
	for(std::map<Variable, std::set<Variable>>::iterator x_iter = phiCongruenceClassMap->begin() ; x_iter != phiCongruenceClassMap->end() ; ++x_iter){
		if(handledVariables.find(x_iter->first) == handledVariables.end()){
			if(isNewSubscriptNeeded){
				isNewSubscriptNeeded = false;
				phiRepresentativeSubscript++;
			}
			Variable currentVariable = x_iter->first;
			LocalVariablePtr renamedVariable = std::make_shared<LocalVariable>(std::string(nameLength, currentName));
			renamedVariable->expressionType = getphiCongruenceClassType(x_iter->second);
			//renamedVariable->size_in_bytes = 1;

			//msg("renaimed variable: %s %s\n", renamedVariable->expressionType->getTypeCOLSTR().c_str(), renamedVariable->getExpressionString().c_str());

			currentName++;
			if(currentName == 0x7b){
				currentName = 'a';
				nameLength++;
			}
			//LocalVariablePtr renamedVariable = std::make_shared<LocalVariable>("var_" + currentVariable.name + "_" + boost::lexical_cast<std::string>(phiRepresentativeSubscript));
			//Instruction* definingInstruction = definitionsMap->operator[](currentVariable.name)[currentVariable.subscript];
			for(std::set<Variable>::iterator xi_iter = x_iter->second.begin() ; xi_iter != x_iter->second.end() ; ++xi_iter){
				Variable xi = *xi_iter;
				std::pair<std::set<Variable>::iterator, bool> ret = handledVariables.insert(xi);
				isNewSubscriptNeeded |= (ret.second == true ?  replaceSimpleVariable(xi, renamedVariable) : false);
			}
		}
	}
}

bool PhiFunctionsEliminator::replaceSimpleVariable(const Variable &var, const ExpressionPtr &renamedVariable){
	//msg("replaceSimpleVariable(%s_%d) --> %s\n", var.name.c_str(), var.subscript, renamedVariable->getExpressionString().c_str());
	//std::string phiRepresentativeName = "local_";
	bool needNewSubscript = false;
	updateFunctionArguments(var, renamedVariable);
	//LocalVariable renamedVariable("var_" + var->name + "_" + boost::lexical_cast<std::string>(phiRepresentativeSubscript));
	if(isVariableDefined(var)){
		//msg("defined\n");
		InstructionPtr definingInstruction = definitionsMap->operator[](var.name).operator[](var.subscript);
		//msg("def inst: %s\n", definingInstruction->getInstructionString().c_str());
		if(definingInstruction->type == PHI_FUNCTION)
			removeInstruction(definingInstruction);
		else{
			//LocalVariable renamedVariable(phiRepresentativeName + boost::lexical_cast<std::string>(phiRepresentativeSubscript));
			//msg("replace definition in %s\n", definingInstruction->getInstructionString().c_str());
			Variable relatedVariable;
			relatedRegistersMonitor->getDefinedRelatedRegister(var.name, var.subscript, relatedVariable);
			needNewSubscript = definingInstruction->replaceDefinition(relatedVariable.name, relatedVariable.subscript, renamedVariable);
		}
	}
	std::vector<Variable> relatedUses;
	relatedRegistersMonitor->getUsedRelatedRegisters(var.name, var.subscript, relatedUses);
	if(!relatedUses.empty()){
		for(std::vector<Variable>::iterator it = relatedUses.begin() ; it != relatedUses.end() ; ++it){
			std::vector<InstructionPtr>* usingInstructions = &(usesMap->operator[](it->name).operator[](it->subscript));
			for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
				InstructionPtr usingInstruction = *use_iter;
				if(usingInstruction->type != PHI_FUNCTION){
					//LocalVariable renamedVariable(var->name + boost::lexical_cast<std::string>(phiRepresentativeSubscript));
					needNewSubscript |= usingInstruction->replaceUse(it->name, it->subscript, renamedVariable);
				}
			}
		}
	}
	return needNewSubscript;
}

void PhiFunctionsEliminator::updateFunctionArguments(const Variable &var, const ExpressionPtr &renamedVariable){
	for(std::vector<ExpressionPtr>::iterator arg_iter = functionArguments->begin() ; arg_iter != functionArguments->end() ; ++arg_iter){
		if((*arg_iter)->isSameVariable(var.name, var.subscript)){
			*arg_iter = renamedVariable;
			break;
		}
	}
}

void PhiFunctionsEliminator::removeInstruction(const InstructionPtr &instructionToRemove){
	InstructionVectorPtr instructions = controlFlowGraph->nodes->at(instructionToRemove->ContainingNodeId)->instructions;
	instructions->erase(std::remove(instructions->begin(), instructions->end(), instructionToRemove) ,instructions->end());
}

bool PhiFunctionsEliminator::isMemoryVariable(const Variable &var){
	bool result = false;
	InstructionPtr definingInstruction = definitionsMap->operator[](var.name).operator[](var.subscript);
	if(isInstructionPointerNull(definingInstruction)){
		//msg("definingInstruction is null\n");
		std::map<int, InstructionPtr>* relatedDefinitions = &(definitionsMap->operator[](var.name));
		for(std::map<int, InstructionPtr>::iterator def_iter = relatedDefinitions->begin() ; def_iter != relatedDefinitions->end() ; ++def_iter){
			if(!isInstructionPointerNull(def_iter->second)){
				Variable relatedVariable(var.name, def_iter->first);
				//msg("relatedVariable %s_%d\n", relatedVariable.name.c_str(), relatedVariable.subscript);
				return isMemoryVariable(relatedVariable, def_iter->second);
			}
		}
		return var.name.size() > 4 ? var.name.substr(0, 4).compare("mem[") == 0 : false;
	}
	return isMemoryVariable(var, definingInstruction);
}

bool PhiFunctionsEliminator::isMemoryVariable(const Variable &var, const InstructionPtr &definingInstruction){
	std::vector<ExpressionPtr> definedElements;
	definingInstruction->getDefinedElements(definedElements);
	if(!definedElements.empty()){
		for(std::vector<ExpressionPtr>::iterator def_iter = definedElements.begin() ; def_iter != definedElements.end() ; ++def_iter){
			ExpressionPtr definedVariable = *def_iter;
			if(definedVariable->isSameVariable(var.name, var.subscript) && (definedVariable->isMemoryVariable())){
				return true;
			}
		}
	}
	return false;
}

void PhiFunctionsEliminator::removeSubscriptOfDefintion(const Variable &var, InstructionPtr &inThisInstruction){
	std::vector<ExpressionPtr> definedElements;
	inThisInstruction->getDefinedElements(definedElements);
	removeSubscriptFromCandidates(var, definedElements);
}

void PhiFunctionsEliminator::removeSubscriptOfUse(const Variable &var, InstructionPtr &inThisInstruction){
	std::vector<ExpressionPtr> usedElements;
	inThisInstruction->getUsedElements(usedElements);
	removeSubscriptFromCandidates(var, usedElements);
}

void PhiFunctionsEliminator::removeSubscriptFromCandidates(const Variable &var, std::vector<ExpressionPtr> &candidates){
	if(!candidates.empty()){
		for(std::vector<ExpressionPtr>::iterator candidate_iter = candidates.begin() ; candidate_iter != candidates.end() ; ++candidate_iter){
			if((*candidate_iter)->isSameVariable(var.name, var.subscript)){
				(*candidate_iter)->subscript = NO_SUBSCRIPT;
			}
		}
	}
}

void PhiFunctionsEliminator::removeZeroSubscripts(){
	std::set<InstructionPtr> handledInstructions;
	for(std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator useName_iter = usesMap->begin() ; useName_iter != usesMap->end() ; ++useName_iter){
		std::vector<InstructionPtr>* uses = &(useName_iter->second.operator[](0));
		handledInstructions.clear();
		for(std::vector<InstructionPtr>::iterator usingInst_iter = uses->begin() ; usingInst_iter != uses->end() ; ++usingInst_iter){
			InstructionPtr usingInstruction = *usingInst_iter;
			if(handledInstructions.find(usingInstruction) == handledInstructions.end()){
				Variable var(useName_iter->first, 0);
				removeSubscriptOfUse(var, usingInstruction);
			}
		}
	}
}

TypePtr PhiFunctionsEliminator::getphiCongruenceClassType(std::set<Variable>& phiCongruenceClass){
	for(std::set<Variable>::iterator iter = phiCongruenceClass.begin() ; iter != phiCongruenceClass.end() ; ++iter){
		TypePtr phiType = typeMap->getType(iter->name, iter->subscript);
		//msg("><><><>< type(%s_%d) = %s\n", iter->name.c_str() , iter->subscript, phiType->getTypeCOLSTR().c_str());
		if(!phiType->isVoidType())
			return phiType;
	}
	return std::make_shared<TopType>();
}
