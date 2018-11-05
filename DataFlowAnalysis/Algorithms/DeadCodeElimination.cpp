////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DeadCodeElimination.h"
#include "../../Shared/InstructionUtilities.h"

DeadCodeElimination::DeadCodeElimination(ControlFlowGraphPtr _controlFlowGraph,
										 definitionsMapPtr _definitionsMap,
										 definitionsMapPtr _flagDefinitionsMap,
										 usesMapPtr _usesMap)
										 
										 : DataFlowAlgorithm(_controlFlowGraph, _definitionsMap, _flagDefinitionsMap, _usesMap)
{
	// dce_out.open("dce.txt");
}


DeadCodeElimination::~DeadCodeElimination(void)
{
}

void DeadCodeElimination::applyAlgorithm(){
	removePhiArgumentsSameAsTarget();
	removeTrivialPhiChains();
	do{
		for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
			std::string defName = name_iter->first;

			for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
				int defSubscript = subscript_iter->first;
				InstructionPtr definingInstruction = subscript_iter->second;
				removeDefinitionIfDead(defName, defSubscript);
			}
		}
	}while(removeTrivialPhiChains());
}

void DeadCodeElimination::removeDefinitionIfDead(const std::string &definitionName, int definitionSubscript){
	//if(definitionName.compare("ebp") == 0 && definitionSubscript == 8)
	//dce_out << "removeDefinitionIfDead(" << definitionName.c_str() << ", " << definitionSubscript << ")\n";
	//dce_out.flush();
	InstructionPtr definingInstruction = getDefiningInstruction(definitionName, definitionSubscript);//(*definitionsMap)[definitionName][definitionSubscript];
	if(isVariableDead(definitionName, definitionSubscript) && !isInstructionPointerNull(definingInstruction)){
		if(definingInstruction->type == CALL){
			CallPtr definingCall = std::dynamic_pointer_cast<Call>(definingInstruction);
			removeDefinitionFromFunctionReturns(definitionName, definitionSubscript, definingCall);
		}
		else if(definingInstruction->type == FLAG_MACRO){
			FlagMacroPtr definingFlagMacro = std::dynamic_pointer_cast<FlagMacro>(definingInstruction);
			removeDeadFlag(definitionName, definitionSubscript, definingFlagMacro);
		}
		else if(canRemoveInstruction(definingInstruction)){
			removeDeadDefinition(definitionName, definitionSubscript, definingInstruction);
		}
	}
}

void DeadCodeElimination::removeDefinitionFromFunctionReturns(const std::string &definitionName, int definitionSubscript, CallPtr &definingFunctionCall){
	ExpressionVectorPtr returns = definingFunctionCall->returns;
	for(std::vector<ExpressionPtr>::iterator ret_iter = returns->begin() ; ret_iter != returns->end() ; ++ret_iter){
		if((*ret_iter)->isSameVariable(definitionName, definitionSubscript)){
			returns->erase(ret_iter);
			definitionsMap->operator[](definitionName).operator[](definitionSubscript).reset();
			break;
		}
	}
}

void DeadCodeElimination::removeDeadFlag(const std::string &deadFlagName, int deadFlagSubscript, FlagMacroPtr &definingFlagMacro){
	FlagSetPtr definedFlags = definingFlagMacro->definedFlags;
	for(std::set<FlagPtr>::iterator flag_iter = definedFlags->begin() ; flag_iter != definedFlags->end() ; ++flag_iter){
		FlagPtr flag = (*flag_iter);
		if(flag->getName().compare(deadFlagName) == 0){
			definedFlags->erase(flag);
			definitionsMap->operator[](deadFlagName).operator[](deadFlagSubscript).reset();
			//(*definitionsMap)[deadFlagName][deadFlagSubscript] = NULL;
			break;
		}
	}
	if(definedFlags->empty()){
		removeInstructionFromUsesMap(definingFlagMacro);
		removeInstruction(definingFlagMacro);
	}
}

void DeadCodeElimination::removeDeadDefinition(const std::string &deadDefinitionName, int deadDefinitionSubscript, const InstructionPtr &definingInstruction){
	definitionsMap->operator[](deadDefinitionName).operator[](deadDefinitionSubscript).reset();

	removeInstructionFromUsesMap(definingInstruction);
	
	//(*definitionsMap)[deadDefinitionName][deadDefinitionSubscript] = NULL;
	updateRelatedRegistersDefinitionsMap_Remove(deadDefinitionName, deadDefinitionSubscript);
	removeInstruction(definingInstruction);
}

bool DeadCodeElimination::canRemoveInstruction(const InstructionPtr &inst){
	std::vector<ExpressionPtr> defs;
	inst->getDefinedElements(defs);
	if(!defs.empty()){
		for(std::vector<ExpressionPtr>::iterator defExp_iter = defs.begin() ; defExp_iter != defs.end() ; ++defExp_iter){
			if(!canRemoveExpression(*defExp_iter)){
				return false;
			}
		}
	}
	return true;
}

bool DeadCodeElimination::canRemoveExpression(const ExpressionPtr &exp){
	if(exp->type == GLOBAL_VARIABLE){
		return false;
	}
	else if(exp->type == POINTER){
		if(exp->subscript == NO_SUBSCRIPT)
			return false;
		//msg("got here (%s)\n", exp->getExpressionString().c_str());
		//return true;
		std::vector<ExpressionPtr> addressExpressionElements;
		createExpressionElements(std::dynamic_pointer_cast<PointerExp>(exp)->addressExpression, false, addressExpressionElements);
		//((PointerExp*)exp)->addressExpression->getExpressionElements(false);
		
		for(std::vector<ExpressionPtr>::iterator addrExp_iter = addressExpressionElements.begin() ; addrExp_iter != addressExpressionElements.end() ; ++addrExp_iter){
			if(isNeitherESPNorConstant((*addrExp_iter))){
				//msg("false\n");
				return false;
			}
		}
		//msg("true\n");
		return true;
	}
	return true;
}

bool DeadCodeElimination::isNeitherESPNorConstant(const ExpressionPtr &exp){
	return !(exp->type == NUMERIC_CONSTANT || (exp->type == REGISTER && std::dynamic_pointer_cast<Register>(exp)->regNo == R_sp));
}

void DeadCodeElimination::removeInstruction(const InstructionPtr &instToBeDeleted){
	InstructionVectorPtr instructions = controlFlowGraph->nodes->operator[](instToBeDeleted->ContainingNodeId)->instructions;
	//(*(controlFlowGraph->nodes))[instToBeDeleted->ContainingNodeId]->instructions;
	instructions->erase(std::remove(instructions->begin(), instructions->end(), instToBeDeleted) ,instructions->end());
}

void DeadCodeElimination::removeExpressionsFromUsingInstruction(std::vector<ExpressionPtr> &expListToBeRemoved, const InstructionPtr &parentInst){
	for(std::vector<ExpressionPtr>::iterator use_iter = expListToBeRemoved.begin() ; use_iter != expListToBeRemoved.end() ; ++use_iter){
		ExpressionPtr useExp = *use_iter;
		std::string useName = useExp->getName();
		int useSubscript = useExp->subscript;
		if(isVariableDefined(useName, useSubscript)){
			if(canRemoveExpression(useExp) && useSubscript != 0){
				removeInstructionFromVariableUsesMap(useName, useSubscript, parentInst);
				removeDefinitionIfDead(useName, useSubscript);
			}
			else{
				removeInstructionFromVariableUsesMap(useName, useSubscript, parentInst);
			}
		}
	}
}

void DeadCodeElimination::removeInstructionFromUsesMap(const InstructionPtr &instToBeDeleted){
	std::vector<ExpressionPtr> usedElements;
	instToBeDeleted->getUsedElements(usedElements);
	if(!usedElements.empty()){
		removeExpressionsFromUsingInstruction(usedElements, instToBeDeleted);
	}
}

void DeadCodeElimination::updateRelatedRegistersDefinitionsMap_Remove(const std::string &name, int subscript){
	InstructionPtr definingInstruction = getDefiningInstruction(name, subscript);//(*definitionsMap)[name][subscript];
	for(std::set<std::string>::iterator reg_iter = relatedRegistersMap[name].begin() ; reg_iter != relatedRegistersMap[name].end() ; ++reg_iter){
		//for(std::map<int, InstructionPtr>::iterator subscript_iter = (*definitionsMap)[*reg_iter].begin() ; subscript_iter != (*definitionsMap)[*reg_iter].end() ; ++subscript_iter){
		for(std::map<int, InstructionPtr>::iterator subscript_iter = definitionsMap->operator[](*reg_iter).begin() ; subscript_iter != definitionsMap->operator[](*reg_iter).end() ; ++subscript_iter){
			if(definingInstruction == subscript_iter->second){
				subscript_iter->second.reset();
				//subscript_iter->second = NULL;
			}
		}
	}
}

void DeadCodeElimination::removePhiArgumentsSameAsTarget(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		for(std::vector<InstructionPtr>::iterator inst_iter = currentNode->instructions->begin() ; inst_iter != currentNode->instructions->end() ; ++inst_iter){
			InstructionPtr currentInstruction = *inst_iter;
			if(currentInstruction->type == PHI_FUNCTION){
				Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(currentInstruction);

				isPhiFunctionWithEqualParameters(phiFunction);

				std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin();
				while(arg_iter != phiFunction->arguments->end()){
					if((*arg_iter)->subscript == phiFunction->target->subscript){
						deleteInstructionFromUsesMap(*arg_iter, phiFunction);
						arg_iter = phiFunction->arguments->erase(arg_iter);
					}
					else
						++arg_iter;
				}
			}
			else{
				break;
			}
		}
	}
}

bool DeadCodeElimination::removeTrivialPhiChains(){
	std::set<InstructionPtr> trivialPhiChainElements, currentTestedPhiSet;
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		std::vector<InstructionPtr>::iterator inst_iter = currentNode->instructions->begin();
		while(inst_iter != currentNode->instructions->end()){
			if((*inst_iter)->type == PHI_FUNCTION){
				Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(*inst_iter);
				currentTestedPhiSet.clear();
				if(trivialPhiChainElements.find(phiFunction) == trivialPhiChainElements.end()){
					if(isInTrivialPhiChain(phiFunction, currentTestedPhiSet)){
						/*msg("trivial phi function %s\n", phiFunction->getInstructionString().c_str());
						for(std::set<InstructionPtr>::iterator iit = currentTestedPhiSet.begin() ; iit != currentTestedPhiSet.end() ; ++iit){
							msg("    %s\n", (*iit)->getInstructionString().c_str());
						}
						msg("\n");*/
						trivialPhiChainElements.insert(currentTestedPhiSet.begin(), currentTestedPhiSet.end());
					}
				}
				++inst_iter;
			}
			else{
				inst_iter = currentNode->instructions->end();
			}
		}
	}
	//msg("trivialPhiChainElements contains %d elements\n", trivialPhiChainElements.size());

	for(std::set<InstructionPtr>::iterator trivialPhiInst_iter = trivialPhiChainElements.begin() ; trivialPhiInst_iter != trivialPhiChainElements.end() ; ++trivialPhiInst_iter){
		InstructionPtr trivialPhiInst = *trivialPhiInst_iter;
		//msg("%s ... trivial\n", trivialPhiInst->getInstructionString().c_str());
		if(trivialPhiInst->type == PHI_FUNCTION){
			Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(trivialPhiInst);
			//msg("%s ... trivial\n", phiFunction->getInstructionString().c_str());
			for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
				std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(*arg_iter);
				//msg("removing %s from uses of %s\n",phiFunction->getInstructionString().c_str(), (*arg_iter)->getExpressionString().c_str());
				std::vector<InstructionPtr>::iterator remove_iter = std::find(usingInstructions->begin(), usingInstructions->end(), phiFunction);
				if(remove_iter != usingInstructions->end()){
					//deleteInstructionFromUsesMap(*arg_iter, phiFunction);

					usingInstructions->erase(remove_iter);
				}
			}
			definitionsMap->operator[](phiFunction->target->getName()).operator[](phiFunction->target->subscript).reset();
			//definitionsMap->operator[](phiFunction->target->getName()).operator[](phiFunction->target->subscript) = NULL;
		}
		else if(trivialPhiInst->type == ASSIGNMENT){
			AssignmentPtr assignment = std::dynamic_pointer_cast<Assignment>(trivialPhiInst);
			std::vector<ExpressionPtr> usedElements;
			assignment->getUsedElements(usedElements);
			for(std::vector<ExpressionPtr>::iterator use_iter = usedElements.begin() ; use_iter != usedElements.end() ; ++use_iter){
				std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(*use_iter);
				//msg("removing %s from uses of %s\n",assignment->getInstructionString().c_str(), (*use_iter)->getExpressionString().c_str());
				std::vector<InstructionPtr>::iterator remove_iter = std::find(usingInstructions->begin(), usingInstructions->end(), assignment);
				if(remove_iter != usingInstructions->end()){
					//deleteInstructionFromUsesMap(*use_iter, assignment);
					usingInstructions->erase(remove_iter);
				}
			}
		}
		//msg("	removing(trivialPhiInst) %s\n", trivialPhiInst->getInstructionString().c_str());
		removeInstruction(trivialPhiInst);
	}
	return !trivialPhiChainElements.empty();
}

bool DeadCodeElimination::isInTrivialPhiChain(const InstructionPtr& inst, std::set<InstructionPtr>& chainElements){
	//msg("isInTrivialPhiChain(%s) = \n", inst->getInstructionString().c_str());
	ExpressionPtr definedElement = std::shared_ptr<Expression>();
	if(inst->type == PHI_FUNCTION)
		definedElement = std::dynamic_pointer_cast<Phi_Function>(inst)->target;//((Phi_Function*)inst)->target;
	else if(inst->type == ASSIGNMENT)
		definedElement = std::dynamic_pointer_cast<Assignment>(inst)->lhsOperand;//((Assignment*)inst)->lhsOperand;
	
	if(!canRemoveExpression(definedElement))
		return false;
	bool result = true;
	if(chainElements.find(inst) == chainElements.end()){
		chainElements.insert(inst);
		if(isOnlyUsedInPhiFunctionsOrCopyStatements(definedElement) && noRelatedRegisterUsed(definedElement->getName(), definedElement->subscript)){
			//if(phiFunction->target->getName().compare("mem[(esp_15 + -4)]") == 0) msg("isOnlyUsedInPhiFunctions(%s) = %d\n", phiFunction->target->getExpressionString().c_str(), 1);
			std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(definedElement);
			for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
				//Phi_Function* usingPhiFunction = (Phi_Function*)(*use_iter);
				//bool b = isInTrivialPhiChain(usingPhiFunction, chainElements);
				//if(phiFunction->target->getName().compare("mem[(esp_15 + -4)]") == 0) msg("isOnlyUsedInPhiFunctions(%s) = %d\n", phiFunction->target->getExpressionString().c_str(), b);
				result &= isInTrivialPhiChain(*use_iter, chainElements);
			}
		}
		else{
			//if(phiFunction->target->getName().compare("mem[(esp_15 + -4)]") == 0) msg("isOnlyUsedInPhiFunctions(%s) = %d\n", phiFunction->target->getExpressionString().c_str(), 0);
			result = false;
		}
	}
	//msg("%d\n", result);
	return result;
}

bool DeadCodeElimination::isPhiFunctionWithEqualParameters(const Phi_FunctionPtr& phi){
	//msg("-------------------\nPhiFunction: %s\n", phi->getInstructionString().c_str());
	std::vector<std::vector<ExpressionPtr>> defChains;
	std::vector<ExpressionPtr> paramDefs;
	//std::map<std::string, std::vector<ExpressionPtr>> defChain;
	for(std::vector<ExpressionPtr>::iterator parm_iter = phi->arguments->begin() ; parm_iter != phi->arguments->end() ; ++parm_iter){
		paramDefs.clear();
		paramDefs.push_back(*parm_iter);

		//msg("Param: %s -> ", (*parm_iter)->getExpressionString().c_str());
		InstructionPtr def_inst = getDefiningInstruction((*parm_iter)->getName(), (*parm_iter)->subscript);
		while(isCopyStatementOfLocalVariables(def_inst)){
			ExpressionPtr src = std::dynamic_pointer_cast<Assignment>(def_inst)->rhsOperand;
			paramDefs.push_back(src);
			
			//msg("%s -> ", src->getExpressionString().c_str());
			//defChain[(*parm_iter)->getExpressionString()].push_back(src);
			def_inst = getDefiningInstruction(src->getName(), src->subscript);
		}
		defChains.push_back(paramDefs);
		//msg("[]\n");
	}

	std::vector<ExpressionPtr> firstChain = defChains.front();
	for(std::vector<ExpressionPtr>::iterator def_iter = firstChain.begin() ; def_iter != firstChain.end() ; ++def_iter){
		bool found = true;
		for(std::vector<std::vector<ExpressionPtr>>::iterator parmChain_iter = defChains.begin() + 1 ; parmChain_iter != defChains.end() ; ++parmChain_iter){
			if(!isInList(*def_iter, *parmChain_iter)){
				found = false;
				break;
			}
		}
		if(found){
			replacePhiFunctionByVariable(phi, *def_iter);
			//msg("YES -> %s\n", (*def_iter)->getExpressionString().c_str());
		}
	}

	//msg("-------------------\n");
	return false;
}

bool DeadCodeElimination::isInList(const ExpressionPtr& var, std::vector<ExpressionPtr>& varList){
	for(std::vector<ExpressionPtr>::iterator elm_iter = varList.begin() ; elm_iter != varList.end() ; ++elm_iter){
		if((*elm_iter)->isSameVariable(var->getName(), var->subscript)){
			return true;
		}
	}
	return false;
}

void DeadCodeElimination::replacePhiFunctionByVariable(const Phi_FunctionPtr& phi, ExpressionPtr& var){
	std::vector<InstructionPtr>* using_insts = getUsingInstructions(phi->target);
	for(std::vector<InstructionPtr>::iterator use_iter = using_insts->begin() ; use_iter != using_insts->end() ; ++use_iter){
		InstructionPtr usingInstruction = *use_iter;
		(*use_iter)->replaceUse(phi->target->getName(), phi->target->subscript, var);
		addInstructionToUsesMap(var, usingInstruction);
	}
	using_insts->clear();

	for(std::vector<ExpressionPtr>::iterator parm_iter = phi->arguments->begin() ; parm_iter != phi->arguments->end() ; ++parm_iter){
		ExpressionPtr parm = *parm_iter;
		if(!parm->isSameVariable(var->getName(), var->subscript)){
			std::vector<InstructionPtr>* using_insts = getUsingInstructions(parm);
			for(std::vector<InstructionPtr>::iterator use_iter = using_insts->begin() ; use_iter != using_insts->end() ; ++use_iter){
				InstructionPtr usingInstruction = *use_iter;
				(*use_iter)->replaceUse(parm->getName(), parm->subscript, var);
				addInstructionToUsesMap(var, usingInstruction);
			}
			using_insts->clear();
		}
	}
}

bool DeadCodeElimination::isCopyStatementOfLocalVariables(const InstructionPtr& inst){
	if(!isInstructionPointerNull(inst) && inst->type == ASSIGNMENT){
		AssignmentPtr assign = std::dynamic_pointer_cast<Assignment>(inst);
		return (assign->lhsOperand->type == REGISTER || assign->lhsOperand->type == LOCAL_VARIABLE) 
			&& (assign->rhsOperand->type == REGISTER || assign->rhsOperand->type == LOCAL_VARIABLE); 
	}
	return false;
}

//bool DeadCodeElimination::isMemoryVariable(Expression* variable){
//	return variable->type == POINTER ? isNotStackVariable((PointerExp*)variable) : variable->type == GLOBAL_VARIABLE; 
//}
//
//bool DeadCodeElimination::isNotStackVariable(PointerExp* pointer){
//	bool foundNonStackExp = false; 
//	std::vector<Expression*>* addressElements = pointer->addressExpression->getExpressionElements(true); 
//	for(std::vector<Expression*>::iterator element_iter = addressElements->begin() ; element_iter != addressElements->end() ; ++element_iter){ 
//		Expression* element = *element_iter; 
//		foundNonStackExp = element->type == NUMERIC_CONSTANT ? false 
//			: element->type == REGISTER && ((Register*)element)->regNo == R_sp ? false 
//			: true; 
//		if(foundNonStackExp) 
//			break; 
//	} 
//	delete addressElements; 
//	return foundNonStackExp;
//}

bool DeadCodeElimination::isOnlyUsedInPhiFunctionsOrCopyStatements(const ExpressionPtr &variable){
	std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(variable);
	/*if(usingInstructions->empty())
		return false;
	else{*/
	for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
		InstructionPtr usingInstruction = *use_iter;
		if(usingInstruction->type != PHI_FUNCTION && !isCopyStatement(usingInstruction))
			return false;
	}
	//}
	return true;
}

bool DeadCodeElimination::isCopyStatement(const InstructionPtr &inst){
	//msg("isCopyStatement(%s)\n", inst->getInstructionString().c_str());
	if(inst->type == ASSIGNMENT){
		AssignmentPtr assignment = std::dynamic_pointer_cast<Assignment>(inst);
		//msg("%d\n", assignment->lhsOperand->getName().compare(assignment->rhsOperand->getName()) == 0);
		if(assignment->lhsOperand->getName().compare(assignment->rhsOperand->getName()) == 0){
			return true;
		}
		else{
			bool test = isESPIncDec(assignment);
			//msg("isESPIncDec(%s)\n", assignment->getInstructionString().c_str());
			//msg(test ? "    true\n" : "    false\n");
			return isESPIncDec(assignment);
		}
	}
	//msg("false\n");
	return false;
}

bool DeadCodeElimination::isESPIncDec(const AssignmentPtr &assign){
	if(assign->lhsOperand->type == REGISTER && assign->lhsOperand->getName().compare("esp") == 0){
		ExpressionPtr rhs = assign->rhsOperand;
		if(rhs->type == ADDITION_EXPRESSION){
			AdditionExpressionPtr add = std::dynamic_pointer_cast<AdditionExpression>(rhs);
			if(add->operands->size() == 2){
				ExpressionPtr op1 = add->operands->at(0);
				ExpressionPtr op2 = add->operands->at(1);
				return (op1->type == REGISTER && op1->getName().compare("esp") == 0 && op2->type == NUMERIC_CONSTANT) 
					|| (op2->type == REGISTER && op2->getName().compare("esp") == 0 && op1->type == NUMERIC_CONSTANT);
					
			}
		}
	}
	return false;
}
