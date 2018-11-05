////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ExpressionPropagation.h"
#include "../../Shared/InstructionUtilities.h"


ExpressionPropagation::ExpressionPropagation(ControlFlowGraphPtr _controlFlowGraph,
											 definitionsMapPtr _definitionsMap,
											 //std::map<std::string, std::map<int, Instruction*>>* _flagDefinitionsMap,
											 usesMapPtr _usesMap
											 /*std::set<Variable>* _phiVariables*/)
											 : DataFlowAlgorithm(_controlFlowGraph, _definitionsMap, std::shared_ptr<std::map<std::string, std::map<int, InstructionPtr>>>(), _usesMap)
{
	//phiVariables = _phiVariables;
	includePointers = false;
	// out.open("ep.txt", std::ofstream::app);
}


ExpressionPropagation::~ExpressionPropagation(void)
{
}


//void ExpressionPropagation::applyAlgorithm(){
//	getReferencedVariables();
//	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
//		std::string defName = name_iter->first;
//		for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
//			int defSubscript = subscript_iter->first;
//			propagateDefinition(defName, defSubscript);	
//		}
//	}
//}

void ExpressionPropagation::applyAlgorithmNoMemoryVariables(){
	msg("ExpressionPropagation.started \n");
	bool done = false;
	while(!done){
		done = true;
		getReferencedVariables();
		for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
			std::string defName = name_iter->first;
			for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
				int defSubscript = subscript_iter->first;
				if(!doesDefiningInstructionContainMemoryVariable(defName, defSubscript)){
					//msg("propagateDefinition(%s, %d)\n", defName.c_str(), defSubscript);
					bool result = propagateDefinition(defName, defSubscript);
					if(result == true)
						done = false;
				}
			}
		}
	}
}

void ExpressionPropagation::applyAlgorithmWithMemoryVariables(){
	bool done = false;
	while(!done){
		done = true;
		getReferencedVariables();
		for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
			std::string defName = name_iter->first;
			for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
				int defSubscript = subscript_iter->first;
				bool result = propagateDefinition(defName, defSubscript);	
				if(result == true)
					done = false;
			}
		}
	}
}


bool ExpressionPropagation::doesDefiningInstructionContainMemoryVariable(const std::string& defName, int defSubscript){
	InstructionPtr definingInstruction = getDefiningInstruction(defName, defSubscript);
	//out << "definingInstruction = " << (definingInstruction.get() ? definingInstruction->getInstructionString().c_str() : "NULL") << std::endl;
	//out << "doesDefiningInstructionContainMemoryVariable(" << defName.c_str() << ", " << defSubscript << ") = ";
	if(definingInstruction.get() != NULL){
		std::vector<ExpressionPtr> instructionElements;
		definingInstruction->getAllElements(instructionElements);
		for(std::vector<ExpressionPtr>::iterator element_iter = instructionElements.begin() ; element_iter != instructionElements.end() ; ++element_iter){
			ExpressionPtr element = *element_iter;
			if(element->type == POINTER || element->type == GLOBAL_VARIABLE || element->type == ADDRESS_EXPRESSION){
				//out << "true\n";
				return true;
			}
		}
	}
	//out << "false\n";
	return false;
}

bool ExpressionPropagation::propagateDefinition(const std::string& defName, int defSubscript){
	bool isSuccessfull = false;
	bool debug = false;//((defName.compare("eax") == 0 || defName.compare("al") == 0) && defSubscript == 9);
	
	if(debug){
		out << "propagateDefinition(" << defName.c_str() << ", " << defSubscript << ")\n";
		out.flush();
	}
	//msg("*******\npropagateDefinition(%s, %d)\n", defName.c_str(), defSubscript);
	InstructionPtr definingInstruction = getDefiningInstruction(defName, defSubscript);
	if(!isVariableDead(defName, defSubscript) && defName.compare("stk_pdwDataLen") != 0){
		if(isAssignment(definingInstruction)){
			if(debug){
				out << "definingInstruction: " << definingInstruction->getInstructionString().c_str() << std::endl;
				out.flush();
			}
			//msg("definingInstruction: %s\n", definingInstruction->getInstructionString().c_str());
			AssignmentPtr definingAssignment = std::dynamic_pointer_cast<Assignment>(definingInstruction);
			if(true || !isAssignmentOfAddressExpression(definingAssignment)){
				std::vector<InstructionPtr> usingInstructionsCopy(usesMap->operator[](defName).operator[](defSubscript)); //((*usesMap)[defName][defSubscript]);
				for(std::vector<InstructionPtr>::iterator use_inst_iter = usingInstructionsCopy.begin() ; use_inst_iter != usingInstructionsCopy.end() ; ++use_inst_iter){			
					InstructionPtr usingInstruction = *use_inst_iter;
					//msg("use: %s\n-------\n", usingInstruction != NULL ? usingInstruction->getInstructionString().c_str() : "NULL");
					PropagationUnitPtr propagationUnit = std::make_shared<PropagationUnit>(defName, defSubscript, definingAssignment, usingInstruction);
					if(canPropagateDefinitionToUse(propagationUnit)){
						//isSuccessfull = propagationUnit->usingInstruction->type != PHI_FUNCTION && propagationUnit->usingInstruction->type != FLAG_MACRO;
						std::string inst_str_before = propagationUnit->usingInstruction->getInstructionString();
						if(debug){
							out << "UsingInstruction(before) " << propagationUnit->usingInstruction->getInstructionString().c_str() << std::endl;
							out.flush();
						}
						//msg("UsingInstruction(before) %s\n", propagationUnit->usingInstruction->getInstructionString().c_str());
						propagateDefinitionToUse(propagationUnit);
						isSuccessfull = inst_str_before.compare(propagationUnit->usingInstruction->getInstructionString()) != 0;
						if(debug){
							out << "UsingInstruction(after) " << propagationUnit->usingInstruction->getInstructionString().c_str() << std::endl;
							out.flush();
						}
						//msg("UsingInstruction(after) %s\n", propagationUnit->usingInstruction->getInstructionString().c_str());
					}
					//msg("got here 2\n");
				}
			}
		}
		/*else if(isFunctionUsedOnlyInCondition(definingInstruction)){
			CallPtr definingFunction = std::dynamic_pointer_cast<Call>(definingFunction);
			definingFunction->returns->clear();
			HighLevelConditionPtr usingHLCondition = std::dynamic_pointer_cast<HighLevelCondition>(usesMap->at(defName).at(defSubscript).at(0));
			usingHLCondition->replaceChildExpression(defName, defSubscript, definingFunction);
		}*/
	}
	if(debug){
		out << (isSuccessfull ? "successful propagation" : "no successful propagation") << std::endl;
		out.flush();
	}
	return isSuccessfull;
	//out.close();
}

void ExpressionPropagation::propagateDefinitionToUse(PropagationUnitPtr& propagationUnit){
	InstructionPtr usingInstruction = propagationUnit->usingInstruction;

	if(usingInstruction->type != PHI_FUNCTION){
		std::vector<ExpressionPtr> addedElements;
		std::vector<ExpressionPtr> replacedElements;
		
		updateUsingInstruction(propagationUnit, addedElements, replacedElements);
		addExpressionsToUsingInstruction(addedElements, usingInstruction);
		updateReplacedExpressionsUsesMap(replacedElements, usingInstruction);
		/*if(canRemoveExpression(propagationUnit->definingAssignment->lhsOperand)){
			removeDeadReplacedExpressions(replacedElements);
		}*/
	}
}

void ExpressionPropagation::updateUsingInstruction(PropagationUnitPtr& propagationUnit, std::vector<ExpressionPtr> &addedElements_out, std::vector<ExpressionPtr> &replacedElements_out){
	//msg("updateUsingInstruction()\n");
	InstructionPtr usingInstruction = propagationUnit->usingInstruction;
	std::vector<ExpressionPtr> usedElements;
	usingInstruction->getUsedElements(usedElements);
	copyVector(usedElements, replacedElements_out);

	removeUndefinedExpressions(replacedElements_out);
	
	ExpressionPtr replacingExpression = propagationUnit->definingAssignment->rhsOperand;
	if(usingInstruction->type == CALL){
		replaceFunctionCallPointer(propagationUnit);
		replaceFunctionCallParameters(propagationUnit);
		//usingInstruction->replaceUsedChildExpression(propagationUnit->definitionName, propagationUnit->definitionSubscript, replacingExpression->deepcopy());
	}
	else{
		//msg("in %s, replace %s_%d by %s\n", usingInstruction->getInstructionString().c_str(), propagationUnit->definitionName.c_str(), propagationUnit->definitionSubscript, replacingExpression->getExpressionString().c_str());
		usingInstruction->replaceUsedChildExpression(propagationUnit->definitionName, propagationUnit->definitionSubscript, replacingExpression->deepcopy());
	}
	//msg("usingInstruction after replacement: %s\n", usingInstruction->getInstructionString().c_str());
	usingInstruction->getUsedElements(addedElements_out);
	//*addedElements_out = usingInstruction->getUsedElements();
	removeUndefinedExpressions(addedElements_out);
	
	computeSymetricDifference(addedElements_out, replacedElements_out);
}


void ExpressionPropagation::addExpressionsToUsingInstruction(std::vector<ExpressionPtr> &expListToBeAdded, const InstructionPtr &parentInst){
	for(std::vector<ExpressionPtr>::iterator expToBeAdded_iter = expListToBeAdded.begin() ; expToBeAdded_iter != expListToBeAdded.end() ; ++expToBeAdded_iter){
		ExpressionPtr expToBeAdded = *expToBeAdded_iter;
		std::string expName = expToBeAdded->getName();
		if(!expName.empty() && definitionsMap->find(expName) != definitionsMap->end()){
			//if to_inst not in Definition_Use_Chain[element.name][element.subscript]:
			usesMap->operator[](expName).operator[](expToBeAdded->subscript).push_back(parentInst);
			//(*usesMap)[expName][expToBeAdded->subscript].push_back(parentInst);
		}
	}
}

bool ExpressionPropagation::canPropagateDefinitionToUse(const PropagationUnitPtr& propagationUnit){
	//return true;

	if(isStackPointer(propagationUnit->definingAssignment->lhsOperand))
		return true;
	std::vector<ExpressionPtr> definitionElements;
	propagationUnit->definingAssignment->lhsOperand->getExpressionElements(true, definitionElements);	
	for(std::vector<ExpressionPtr>::iterator it = definitionElements.begin() ; it != definitionElements.end() ; ++it){
		if(isStackPointer(*it))
			return true;
	}

	if(false){
		msg("Propagating %s: %s -> %s\n", propagationUnit->definingAssignment->lhsOperand->getExpressionString().c_str(), 
										  propagationUnit->definingAssignment->getInstructionString().c_str(),
										  propagationUnit->usingInstruction->getInstructionString().c_str());
		msg("\t%s -> %d\n", "doesPropagationResultInMultipePointers", doesPropagationResultInMultipePointers(propagationUnit));
		msg("\t%s -> %d\n", "doesPropagatePhiResourceIntoConditionalThroughOverwritingStatement", doesPropagatePhiResourceIntoConditionalThroughOverwritingStatement(propagationUnit));
		msg("\t%s -> %d\n", "isAssignmentWithMultipleUsesAndMultipleExpressions", isAssignmentWithMultipleUsesAndMultipleExpressions(propagationUnit->definingAssignment));
		msg("\t%s -> %d\n", "doesPropagationResultExceedMaximumExpressionLength", doesPropagationResultExceedMaximumExpressionLength(propagationUnit));
		msg("\t%s -> %d\n", "doesPropagationSimplifyInstruction", doesPropagationSimplifyInstruction(propagationUnit));
		msg("\t%s -> %d\n", "hasAlias", hasAlias(propagationUnit->definitionName, propagationUnit->definitionSubscript));
		msg("\t%s -> %d\n", "doesPropagatePhiResourceOverPhiFunction", doesPropagatePhiResourceOverPhiFunction(propagationUnit));
		msg("\t%s -> %d\n", "isAssignmentWithPointerOfLessThanDWord", isAssignmentWithPointerOfLessThanDWord(propagationUnit->definingAssignment));
	}

	return !doesPropagationResultInMultipePointers(propagationUnit)
			&& !doesPropagatePhiResourceIntoConditionalThroughOverwritingStatement(propagationUnit)
			&& ((!isAssignmentWithMultipleUsesAndMultipleExpressions(propagationUnit->definingAssignment) && !doesPropagationResultExceedMaximumExpressionLength(propagationUnit))
				|| doesPropagationSimplifyInstruction(propagationUnit))
			&& !hasAlias(propagationUnit->definitionName, propagationUnit->definitionSubscript)
			&& !doesPropagatePhiResourceOverPhiFunction(propagationUnit)
			//&& isAssignmentWithEqualSizedOperands(propagationUnit->definingAssignment)
			&& !isAssignmentWithPointerOfLessThanDWord(propagationUnit->definingAssignment)
			&& propagationUnit->definingAssignment->lhsOperand->type != GLOBAL_VARIABLE
			/*&& (propagationUnit->usingInstruction->type != CALL ? propagationUnit->usingInstruction->getNumberOfUsedExpressions() <= MAXIMUM_EXPRESSION_LENGTH : true)*/
			/*&& propagationUnit->definingAssignment->rhsOperand->type != TERNARY_EXPRESSION*/;
	//TODO: Handle overwriting expressions
}

bool ExpressionPropagation::isAssignmentWithMultipleUsesAndMultipleExpressions(const AssignmentPtr& assignment){
	if(isStackPointer(assignment->lhsOperand)){
		return false;
	}
	std::vector<InstructionPtr> highLevelUses;
	std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(assignment->lhsOperand);
	for(std::vector<InstructionPtr>::iterator u_iter = usingInstructions->begin() ; u_iter != usingInstructions->end() ; ++u_iter){
		if((*u_iter)->type != FLAG_MACRO && (*u_iter)->type != PHI_FUNCTION){
			highLevelUses.push_back(*u_iter);
		}
	}
	//return num_uses > 1;
	if(highLevelUses.size() > 1){
		//msg("%s has %d usages\n", assignment->getInstructionString().c_str(), highLevelUses.size());
		//msg("isAssignmentWithConstantUpdate(%d)\n", isAssignmentWithConstantUpdate(assignment));
		//msg("haveUsesOtherThanVariableOrConstant(%d)\n",haveUsesOtherThanVariableOrConstant(assignment->lhsOperand, highLevelUses));
		if(isAssignmentWithConstantUpdate(assignment) && !haveUsesOtherThanVariableOrConstant(assignment->lhsOperand, highLevelUses)){
			//msg("    only ofsets\n");
			return false;
		}

		std::vector<ExpressionPtr> rhs_elements;
		assignment->rhsOperand->getExpressionElements(true, rhs_elements);
		return getNumberOfSimpleExpressions(rhs_elements) > 1;
		 
		/*int num_exprs = 0;
		for(std::vector<ExpressionPtr>::iterator expr_iter = rhs_elements.begin() ; expr_iter != rhs_elements.end() ; ++expr_iter){
			if((*expr_iter)->isSimpleExpression)
				num_exprs++;
		}
		return num_exprs > 1;*/
	}
	return false;
}

bool ExpressionPropagation::haveUsesOtherThanVariableOrConstant(const ExpressionPtr& var, std::vector<InstructionPtr> &inst_list){
	std::vector<ExpressionPtr> usedExps;
	for(std::vector<InstructionPtr>::iterator inst_iter = inst_list.begin() ; inst_iter != inst_list.end() ; ++inst_iter){
		usedExps.clear();
		(*inst_iter)->getUsedElements(usedExps);
		if(!useOnlySameVariableOrConstant(var, usedExps))
			return true;
	}
	return false;
}

bool ExpressionPropagation::isAssignmentWithEqualSizedOperands(const AssignmentPtr& assignment){
	return assignment->lhsOperand->size_in_bytes == assignment->rhsOperand->size_in_bytes;
}

bool ExpressionPropagation::isAssignmentWithPointerOfLessThanDWord(const AssignmentPtr& assignment){
	return assignment->rhsOperand->type == POINTER && assignment->rhsOperand->size_in_bytes < 4;
}

bool ExpressionPropagation::doesPropagatePhiResourceOverPhiFunction(const PropagationUnitPtr& propagationUnit){
	std::vector<ExpressionPtr> propagatedElements;
	createExpressionElements(propagationUnit->definingAssignment->rhsOperand, true, propagatedElements);
	for(std::vector<ExpressionPtr>::iterator element_iter = propagatedElements.begin() ; element_iter != propagatedElements.end() ; ++element_iter){
		ExpressionPtr element = *element_iter;
		std::vector<InstructionPtr>* usingInstructions = getUsingInstructions(element);
		for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
			InstructionPtr using_inst = *use_iter;
			if(using_inst->type == PHI_FUNCTION && controlFlowGraph->cfgPathExists(propagationUnit->definingAssignment, propagationUnit->usingInstruction, using_inst)){
				return true;
			}
		}
	}
	return false;
}

bool ExpressionPropagation::doesPropagationResultInMultipePointers(const PropagationUnitPtr& propagationUnit/*std::string defName, int defSubscript, Expression* expToBePropagated, Instruction* usingInstruction*/){
	//msg("canPropagateDefinitionToUse()\n");
	ExpressionPtr expToBePropagated = propagationUnit->definingAssignment->rhsOperand;
	if(expToBePropagated->type == POINTER || expToBePropagated->doesExpressionUseAPointer()){
		return isDefinitionUsedInPointerInUsingInstruction(propagationUnit);
	}
	return false;
}

bool ExpressionPropagation::doesPropagatePhiResourceIntoConditionalThroughOverwritingStatement(const PropagationUnitPtr& propagationUnit){
	//msg("doesPropagatePhiResourceIntoConditionalThroughOverwritingStatement()\n");
	//TODO this is only for testing one example
	//return false;
	//if(propagationUnit->usingInstruction->type != CONDITIONAL_JUMP)
	//	return false;
	std::vector<ExpressionPtr> propagatedElements;
	createExpressionElements(propagationUnit->definingAssignment->rhsOperand, true, propagatedElements);
	for(std::vector<ExpressionPtr>::iterator element_iter = propagatedElements.begin() ; element_iter != propagatedElements.end() ; ++element_iter){
		ExpressionPtr element = *element_iter;
		//msg("propagatedElement(%s)\n", element->getExpressionString().c_str());
		InstructionPtr definingInstruction = getDefiningInstruction(element->getName(), element->subscript);//definitionsMap->operator[](element->getName())[element->subscript];
		if(!isInstructionPointerNull(definingInstruction) && definingInstruction->type == PHI_FUNCTION){
			//msg("\tdef = %s\n", definingInstruction->getInstructionString().c_str());
			std::vector<ExpressionPtr> arguments;
			Phi_FunctionPtr definingPhiFunction = std::dynamic_pointer_cast<Phi_Function>(definingInstruction);
			getPhiArgumentChain(definingPhiFunction , arguments);
			//for(std::vector<ExpressionPtr>::iterator arg_iter = definingPhiFunction->arguments->begin() ; arg_iter != definingPhiFunction->arguments->end() ; ++arg_iter){
			for(std::vector<ExpressionPtr>::iterator arg_iter = arguments.begin() ; arg_iter != arguments.end() ; ++arg_iter){
				InstructionPtr argDefInst = getDefiningInstruction((*arg_iter)->getName(), (*arg_iter)->subscript);//definitionsMap->operator[]((*arg_iter)->getName())[(*arg_iter)->subscript];
				if(!isInstructionPointerNull(argDefInst) && argDefInst->isOverwritingStatement() &&
					(propagationUnit->definingAssignment == argDefInst || controlFlowGraph->cfgPathExists(propagationUnit->definingAssignment, propagationUnit->usingInstruction, argDefInst))){
						//delete propagatedElements;
						return true;
				}
			}
		}
	}
	//delete propagatedElements;
	return false;
}

void ExpressionPropagation::getPhiArgumentChain(const Phi_FunctionPtr& phi, std::vector<ExpressionPtr>& expChain){
	for(std::vector<ExpressionPtr>::iterator iter = phi->arguments->begin() ; iter != phi->arguments->end() ; ++iter){
		ExpressionPtr arg = *iter;
		if(!isInExpressionList(expChain, arg)){
			expChain.push_back(arg);
			InstructionPtr def_inst = getDefiningInstruction(arg->getName(), arg->subscript);
			if(!isInstructionPointerNull(def_inst) && def_inst->type == PHI_FUNCTION){
				Phi_FunctionPtr def_phi = std::dynamic_pointer_cast<Phi_Function>(def_inst);
				getPhiArgumentChain(def_phi, expChain);
			}
		}
	}
}

bool ExpressionPropagation::doesPropagationResultExceedMaximumExpressionLength(const PropagationUnitPtr& propagationUnit){
	if(propagationUnit->usingInstruction->type == CALL)
		return false;

	std::vector<ExpressionPtr> elements;
	createExpressionElements(propagationUnit->definingAssignment->rhsOperand, true, elements);
	if(doesIncludeStackPointer(elements))
		return false;

	std::vector<ExpressionPtr> lhsElements, rhsElements;
	propagationUnit->definingAssignment->lhsOperand->getExpressionElements(true, lhsElements);
	propagationUnit->definingAssignment->rhsOperand->getExpressionElements(true, rhsElements);
	if(rhsElements.size() <= lhsElements.size())
		return false;
	
	if(propagationUnit->usingInstruction->type == ASSIGNMENT){
		std::vector<ExpressionPtr> rhs_elements;	
		createExpressionElements(std::dynamic_pointer_cast<Assignment>(propagationUnit->usingInstruction)->rhsOperand, true, rhs_elements);
		for(std::vector<ExpressionPtr>::iterator elm_iter = rhs_elements.begin() ; elm_iter != rhs_elements.end() ; ++elm_iter){
			//msg("isSameVariable(%s, %s)\n", propagationUnit->definingAssignment->lhsOperand->getExpressionString().c_str(), (*elm_iter)->getExpressionString().c_str());
			if(propagationUnit->definingAssignment->lhsOperand->isSameVariable((*elm_iter)->getName(), (*elm_iter)->subscript)){
				std::vector<ExpressionPtr> total_elements = elements;
				total_elements.insert(total_elements.end(), rhs_elements.begin(), rhs_elements.end());
				//msg("propagating %s (%d total elements)\n", propagationUnit->definingAssignment->getInstructionString().c_str(), getNumberOfSimpleExpressions(total_elements));
				return getNumberOfSimpleExpressions(total_elements) > MAXIMUM_EXPRESSION_LENGTH + 1;
				//return getNumberOfSimpleExpressions(elements) + getNumberOfSimpleExpressions(rhs_elements) > MAXIMUM_EXPRESSION_LENGTH;
			}
		}


		std::vector<ExpressionPtr> lhs_elements;	
		createExpressionElements(std::dynamic_pointer_cast<Assignment>(propagationUnit->usingInstruction)->lhsOperand, true, lhs_elements);
		for(std::vector<ExpressionPtr>::iterator elm_iter = lhs_elements.begin() ; elm_iter != lhs_elements.end() ; ++elm_iter){
			if(propagationUnit->definingAssignment->lhsOperand->isSameVariable((*elm_iter)->getName(), (*elm_iter)->subscript)){
				std::vector<ExpressionPtr> total_elements = elements;
				total_elements.insert(total_elements.end(), lhs_elements.begin(), lhs_elements.end());
				//msg("propagating %s (%d total elements)\n", propagationUnit->definingAssignment->getInstructionString().c_str(), getNumberOfSimpleExpressions(total_elements));
				return getNumberOfSimpleExpressions(total_elements) > MAXIMUM_EXPRESSION_LENGTH + 1;
				//return getNumberOfSimpleExpressions(elements) + getNumberOfSimpleExpressions(lhs_elements) > MAXIMUM_EXPRESSION_LENGTH;
			}
		}
	}
	
	if(propagationUnit->usingInstruction->type != CALL){
		propagationUnit->usingInstruction->getUsedElements(elements);
	}
	for(std::vector<ExpressionPtr>::iterator iter = elements.begin() ; iter != elements.end() ; ++iter){
		if(isStackPointer(*iter)){
			return false;
		}
	}
	if(getNumberOfSimpleExpressions(elements) > MAXIMUM_EXPRESSION_LENGTH){
		//msg("exceed max length (%d): %s\n", elements.size(), propagationUnit->definingAssignment->getInstructionString().c_str());
		//msg("      using instructions: %s\n", propagationUnit->usingInstruction->getInstructionString().c_str());
		return true;
	}
	return false;
}

bool ExpressionPropagation::doesPropagationSimplifyInstruction(const PropagationUnitPtr& propagationUnit){
	//return false;
	InstructionPtr usingInstCopy = propagationUnit->usingInstruction->deepcopy();
	std::vector<ExpressionPtr> subExps;
	usingInstCopy->getUsedElements(subExps);
	int n1 = getNumberOfSimpleExpressions(subExps);
	subExps.clear();
	usingInstCopy->replaceUsedChildExpression(propagationUnit->definitionName, 
											  propagationUnit->definitionSubscript, 
											  propagationUnit->definingAssignment->rhsOperand->deepcopy());
	usingInstCopy->getUsedElements(subExps);
	return getNumberOfSimpleExpressions(subExps) < n1;
}

bool ExpressionPropagation::isStackPointer(const ExpressionPtr& expr){
	if(expr->type == REGISTER){
		RegisterPtr reg = std::dynamic_pointer_cast<Register>(expr);
		return reg->regNo == R_sp;
	}
	return false;
}

bool ExpressionPropagation::doesIncludeStackPointer(std::vector<ExpressionPtr> &expr_list){
	for(std::vector<ExpressionPtr>::iterator iter = expr_list.begin() ; iter != expr_list.end() ; ++iter){
		if(isStackPointer(*iter))
			return true;
	}
	return false;
}

int ExpressionPropagation::getNumberOfSimpleExpressions(std::vector<ExpressionPtr> &expr_list){
	int num = 0;
	bool found_constant = false;
	for(std::vector<ExpressionPtr>::iterator iter = expr_list.begin() ; iter != expr_list.end() ; ++iter){
		if((*iter)->isSimpleExpression){
			if((*iter)->type == NUMERIC_CONSTANT){
				if(!found_constant){
					num++;
					found_constant = true;
				}
			}
			else{
				num++;
			}
		}
	}
	return num;
}

bool ExpressionPropagation::useOnlySameVariableOrConstant(const ExpressionPtr& var, std::vector<ExpressionPtr> &expr_list){
	for(std::vector<ExpressionPtr>::iterator op_iter = expr_list.begin() ; op_iter != expr_list.end() ; ++op_iter){
		if((*op_iter)->type != NUMERIC_CONSTANT && (*op_iter)->getName().compare(var->getName()) != 0)
			return false;
	}
	return true;
}

//bool ExpressionPropagation::doesContainPhiResource(Expression* expression){
//	bool result = false;
//	std::vector<Expression*>* elements = expression->getExpressionElements(true);
//	for(std::vector<Expression*>::iterator element_iter = elements->begin() ; element_iter != elements->end() ; ++element_iter){
//		//msg("element(%s), phiSourceBlockId(%d)\n", (*element_iter)->getExpressionString().c_str(), (*element_iter)->phiSourceBlockId);
//		if(phiVariables->find(Variable((*element_iter)->getName(), (*element_iter)->subscript)) != phiVariables->end()){
//			result = true;
//			break;
//		}
//	}
//	delete elements;
//	return result;
//}

bool ExpressionPropagation::isDefinitionUsedInPointerInUsingInstruction(const PropagationUnitPtr& propagationUnit){
	std::vector<ExpressionPtr> usingElements;
	propagationUnit->usingInstruction->getAllElements(usingElements);
	for(std::vector<ExpressionPtr>::iterator usingElement_iter = usingElements.begin() ; usingElement_iter != usingElements.end() ; ++usingElement_iter){
		ExpressionPtr usingElement = *usingElement_iter;
		if(usingElement->type == POINTER){
			if(isDefinitionUsedInPointer(propagationUnit, std::dynamic_pointer_cast<PointerExp>(usingElement))){
				return true;
			}
		}
	}
	return false;
}

bool ExpressionPropagation::isDefinitionUsedInPointer(const PropagationUnitPtr& propagationUnit, const PointerExpPtr& pointerExpression){
	std::vector<ExpressionPtr> addressElements;
	createExpressionElements(pointerExpression->addressExpression, true, addressElements);
	for(std::vector<ExpressionPtr>::iterator addressElement_iter = addressElements.begin() ; addressElement_iter != addressElements.end() ; ++addressElement_iter){
		if((*addressElement_iter)->isSameVariable(propagationUnit->definitionName, propagationUnit->definitionSubscript)){
			return true;
		}
	}
	return false;
}

bool ExpressionPropagation::isAssignment(const InstructionPtr& instruction){
	return !isInstructionPointerNull(instruction) && instruction->type == ASSIGNMENT;
}

bool ExpressionPropagation::isAssignmentWithConstantUpdate(const AssignmentPtr& assignment){
	if(assignment->lhsOperand->isSimpleExpression && assignment->rhsOperand->type == ADDITION_EXPRESSION){
		AdditionExpressionPtr rhs_add = std::dynamic_pointer_cast<AdditionExpression>(assignment->rhsOperand);
		for(std::vector<ExpressionPtr>::iterator op_iter = rhs_add->operands->begin() ; op_iter != rhs_add->operands->end() ; ++op_iter){
			if((*op_iter)->type != NUMERIC_CONSTANT && (*op_iter)->getName().compare(assignment->lhsOperand->getName()) != 0)
				return false;
		}
		return true;
	}
	return false;
}

void ExpressionPropagation::removeUndefinedExpressions(std::vector<ExpressionPtr> &fromThisVector){
	std::vector<ExpressionPtr>::iterator exp_iter = fromThisVector.begin();
	while(exp_iter != fromThisVector.end()){
		ExpressionPtr exp = *exp_iter;
		exp_iter = (exp->getName().empty() || exp->subscript == NO_SUBSCRIPT) ? fromThisVector.erase(exp_iter) : exp_iter + 1;
	}
}

void ExpressionPropagation::computeSymetricDifference(std::vector<ExpressionPtr> &firstVector, std::vector<ExpressionPtr> &secondVector){
	std::vector<ExpressionPtr>::iterator first_iter = firstVector.begin();
	std::vector<ExpressionPtr>::iterator second_iter;
	while(first_iter != firstVector.end()){
		second_iter = getPosition(*first_iter, secondVector);
		if(second_iter != secondVector.end()){
			first_iter = firstVector.erase(first_iter);
			second_iter = secondVector.erase(second_iter);
		}
		else{
			++first_iter;
		}
	}
}

std::vector<ExpressionPtr>::iterator ExpressionPropagation::getPosition(const ExpressionPtr &expression, std::vector<ExpressionPtr> &expressionVector){
	for(std::vector<ExpressionPtr>::iterator iter = expressionVector.begin() ; iter != expressionVector.end() ; ++iter){
		ExpressionPtr currentExpression = *iter;
		if(expression->isSameVariable(currentExpression->getName(), currentExpression->subscript)){
			return iter;
		}
	}
	return expressionVector.end();
}

//void ExpressionPropagation::removeDeadReplacedExpressions(std::vector<Expression*>* replacedExpressions){
//	//std::vector<Expression*>* replacedExpressionElements = replacedExpression->getExpressionElements(includePointers);
//	//msg("elementsToRemove: "); printVector(replacedExpressions);
//	for(std::vector<Expression*>::iterator replacedElement_iter = replacedExpressions->begin() ; replacedElement_iter != replacedExpressions->end() ; ++replacedElement_iter){
//		Expression* replacedElement = *replacedElement_iter;
//		std::string replacedElementName = replacedElement->getName();
//		int replacedElementSubscript = replacedElement->subscript;
//		//msg("usingMap[%s][%d]: ", replacedElementName.c_str(), replacedElementSubscript); printInstructionVector(&(*usesMap)[replacedElementName][replacedElementSubscript]);
//		Instruction* definingInstruction = getDefiningInstruction(replacedElementName, replacedElementSubscript);//definitionsMap->operator[](replacedElementName)[replacedElementSubscript];
//		if(!replacedElement->getName().empty() && replacedElement->subscript != NO_SUBSCRIPT){
//			if(definingInstruction != NULL && (*usesMap)[replacedElementName][replacedElementSubscript].empty() && noRelatedRegisterUsed(replacedElementName, replacedElementSubscript)){
//				removeInstructionFromUsesMap(definingInstruction);
//				(*definitionsMap)[replacedElementName][replacedElementSubscript] = NULL;
//				updateRelatedRegistersDefinitionsMap_Remove(replacedElementName, replacedElementSubscript);
//				removeInstruction(definingInstruction);
//			}
//		}
//	}
//	//delete replacedExpressionElements;
//}

void ExpressionPropagation::updateReplacedExpressionsUsesMap(std::vector<ExpressionPtr> &removedExpressions, const InstructionPtr &usingInstruction){
	for(std::vector<ExpressionPtr>::iterator exp_iter = removedExpressions.begin() ; exp_iter != removedExpressions.end() ; ++exp_iter){
		ExpressionPtr removedExpression = *exp_iter;
		removeInstructionFromVariableUsesMap(removedExpression->getName(), removedExpression->subscript, usingInstruction);
		/*std::vector<Instruction*>* usingInstructionList = &((*usesMap)[removedExpression->getName()][removedExpression->subscript]);
		std::vector<Instruction*>::iterator del_iter = std::find(usingInstructionList->begin(), usingInstructionList->end(), usingInstruction);
		if(del_iter != usingInstructionList->end()){
			usingInstructionList->erase(del_iter);
		}	*/
	}
}

void ExpressionPropagation::copyVector(std::vector<ExpressionPtr> &srcVector, std::vector<ExpressionPtr> &dstVector){
	for(std::vector<ExpressionPtr>::iterator iter = srcVector.begin() ; iter != srcVector.end() ; ++iter){
		dstVector.push_back((*iter)->deepcopy());
	}
}

void ExpressionPropagation::replaceFunctionCallPointer(PropagationUnitPtr &propagationUnit){
	//TODO Test ==> needs more consideration
	CallPtr usingCallInstruction = std::dynamic_pointer_cast<Call>(propagationUnit->usingInstruction);
	if(usingCallInstruction->functionPointer->isSameVariable(propagationUnit->definitionName, propagationUnit->definitionSubscript)){
		ExpressionPtr replacingExpression = propagationUnit->definingAssignment->rhsOperand;

		/*out << "replacingInstruction: " << replacingExpression->getExpressionString().c_str() << ", type: " << replacingExpression->type << std::endl;
		if(replacingExpression->type == GLOBAL_VARIABLE)
			out << "----- at " << std::hex << std::dynamic_pointer_cast<GlobalVariable>(replacingExpression)->address;*/

		usingCallInstruction->functionPointer = replacingExpression->deepcopy();
	}
}

void ExpressionPropagation::replaceFunctionCallParameters(PropagationUnitPtr &propagationUnit){
	ExpressionPtr replacingExpression = propagationUnit->definingAssignment->rhsOperand;
	ExpressionVectorPtr parameters = std::dynamic_pointer_cast<Call>(propagationUnit->usingInstruction)->parameters;
	
	for(std::vector<ExpressionPtr>::iterator param_iter = parameters->begin() ; param_iter != parameters->end() ; ++param_iter){
		ExpressionPtr parameter = *param_iter;
		if(parameter->isSameVariable(propagationUnit->definitionName, propagationUnit->definitionSubscript)){
			*param_iter = replacingExpression->deepcopy();
		}
		else{
			parameter->replaceChildExpression(propagationUnit->definitionName, propagationUnit->definitionSubscript, replacingExpression->deepcopy());
		}
		propagationUnit->usingInstruction->replaceBySimplifiedExpression(*param_iter);
	}
}

void ExpressionPropagation::printVector(std::vector<Expression*>* vec){
	for(std::vector<Expression*>::iterator iter = vec->begin() ; iter != vec->end() ; ++iter){
		msg("%s /", (*iter)->getExpressionString().c_str());
	}
	msg("\n");
}

void ExpressionPropagation::printInstructionVector(std::vector<Instruction*>* vec){
	for(std::vector<Instruction*>::iterator iter = vec->begin() ; iter != vec->end() ; ++iter){
		msg("%s /", (*iter)->getInstructionString().c_str());
	}
	msg("\n");
}

//bool ExpressionPropagation::isFunctionUsedOnlyInCondition(const InstructionPtr &inst){
//	if(inst->type == CALL){
//		CallPtr functionCall = std::dynamic_pointer_cast<Call>(inst);
//		if(functionCall->returns->size() == 1){
//			ExpressionPtr def = functionCall->returns->at(0);
//			std::vector<InstructionPtr>* usingInstructions = &(usesMap->at(def->getName()).at(def->subscript));
//			if(usingInstructions->size() == 1){
//				return usingInstructions->at(0)->type == HIGHLEVEL_CONDITION;
//			}
//		}
//	}
//	return false;
//}


bool ExpressionPropagation::hasAlias(const std::string& variableName, int variableSubscript){
	//return false;
	return referencedVariables.find(Variable(variableName, variableSubscript)) != referencedVariables.end();

	std::vector<InstructionPtr>* usingInstructions = &(usesMap->at(variableName).at(variableSubscript));
	for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
		InstructionPtr usingInstruction = *use_iter;
		if(usingInstruction->type == ASSIGNMENT){
			AssignmentPtr usingAssignment = std::dynamic_pointer_cast<Assignment>(usingInstruction);
			if(isAssignmentOfAddressExpression(usingAssignment)){
				ExpressionPtr poitedToVariable = std::dynamic_pointer_cast<AddressExpression>(usingAssignment->rhsOperand)->operand;
				if(poitedToVariable->isSameVariable(variableName, variableSubscript))
					return true;
			}
		}
	}
	return false;
}

bool ExpressionPropagation::isAssignmentOfAddressExpression(const AssignmentPtr& assignment){
	//return false;
	return assignment->rhsOperand->type == ADDRESS_EXPRESSION;
}

void ExpressionPropagation::getReferencedVariables(){
	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
		std::string defName = name_iter->first;
		for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
			int defSubscript = subscript_iter->first;
			if(isAssignment(subscript_iter->second)){
				AssignmentPtr definindAssignment = std::dynamic_pointer_cast<Assignment>(subscript_iter->second);
				if(isAssignmentOfAddressExpression(definindAssignment)){
					//msg("AssignmentOfAddressExpression: %s\n", definindAssignment->getInstructionString().c_str());
					ExpressionPtr referencedVariable = std::dynamic_pointer_cast<AddressExpression>(definindAssignment->rhsOperand)->operand;
					referencedVariables.insert(Variable(referencedVariable->getName(), referencedVariable->subscript));
				}
			}
		}
	}
}
