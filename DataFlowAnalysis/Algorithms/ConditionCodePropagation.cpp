////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ConditionCodePropagation.h"


ConditionCodePropagation::ConditionCodePropagation(ControlFlowGraphPtr _controlFlowGraph,
												   definitionsMapPtr _definitionsMap,
												   definitionsMapPtr _flagDefinitionsMap,
												   usesMapPtr _usesMap)
										 
												   : DataFlowAlgorithm(_controlFlowGraph, _definitionsMap, _flagDefinitionsMap, _usesMap)
{
}


ConditionCodePropagation::~ConditionCodePropagation(void)
{
}


void ConditionCodePropagation::applyAlgorithm(){
	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = flagDefinitionsMap->begin() ; name_iter != flagDefinitionsMap->end() ; ++name_iter){
		replaceFlagUsesByEquivalentConditions(name_iter->first);
	}
}

void ConditionCodePropagation::replace_remaining_flag_macros(){
	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = flagDefinitionsMap->begin() ; name_iter != flagDefinitionsMap->end() ; ++name_iter){
		for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
			InstructionPtr definingInstruction = getDefiningInstruction(name_iter->first, subscript_iter->first);
			if(definingInstruction.get() != NULL){
				if(definingInstruction->type == FLAG_MACRO){
					FlagMacroPtr definingMacro = std::dynamic_pointer_cast<FlagMacro>(definingInstruction);

				}
			}
		}
	}
}

void ConditionCodePropagation::replaceFlagUsesByEquivalentConditions(const std::string& flagName){
	std::map<int, std::vector<InstructionPtr>>* usingInstructionsMap = &(usesMap->operator[](flagName));//&((*usesMap)[flagName]);
	for(std::map<int, std::vector<InstructionPtr>>::iterator use_list_iter = usingInstructionsMap->begin() ; use_list_iter != usingInstructionsMap->end() ; ++use_list_iter){
		int flagSubscript = use_list_iter->first;
		std::vector<InstructionPtr> usingInstructionsCopy(use_list_iter->second);
		for(std::vector<InstructionPtr>::iterator use_inst_iter = usingInstructionsCopy.begin() ; use_inst_iter != usingInstructionsCopy.end() ; ++use_inst_iter){
			InstructionPtr usingInstruction = *use_inst_iter;

			if(usingInstruction->type == CONDITIONAL_JUMP){
				replaceLowLevelConditionInConditionalJump(std::dynamic_pointer_cast<ConditionalJump>(usingInstruction));
			}
			else if(usingInstruction->type == ASSIGNMENT){
				replaceLowLevelConditionInAssignment(std::dynamic_pointer_cast<Assignment>(usingInstruction));
			}
			/*else if(usingInstruction->type == PHI_FUNCTION){
				Phi_FunctionPtr usingPhi = std::dynamic_pointer_cast<Phi_Function>(usingInstruction);
				for(std::vector<ExpressionPtr>::iterator it = usingPhi->arguments->begin() ; it != usingPhi->arguments->end() ; ++it){
					
				}
			}*/
		}
	}
}

void ConditionCodePropagation::replaceLowLevelConditionInConditionalJump(ConditionalJumpPtr usingConditionalJump){
	if(usingConditionalJump->condition->type == LOWLEVEL_CONDITION){
		replaceLowLevelCondition(usingConditionalJump->condition, usingConditionalJump);
	}
}

void ConditionCodePropagation::replaceLowLevelConditionInAssignment(AssignmentPtr usingAssignment){
	if(usingAssignment->rhsOperand->type == TERNARY_EXPRESSION){
		TernaryExpressionPtr ternaryExpression = std::dynamic_pointer_cast<TernaryExpression>(usingAssignment->rhsOperand);
		if(ternaryExpression->firstOperand->type == LOWLEVEL_CONDITION){
			replaceLowLevelCondition(ternaryExpression->firstOperand, usingAssignment);
		}
	}
}

void ConditionCodePropagation::replaceLowLevelCondition(ExpressionPtr& conditionPtr, InstructionPtr usingInstruction){
	HighLevelConditionPtr highLevelCondition = translateCondition(std::dynamic_pointer_cast<LowLevelCondition>(conditionPtr));
	if(!isExpressionPointerNull(highLevelCondition)){
		std::vector<ExpressionPtr> addedElements;
		createExpressionElements(highLevelCondition, true, addedElements);	//highLevelCondition->getExpressionElements(true, addedElements);
		//std::vector<Expression*>* addedElements = highLevelCondition->getExpressionElements(true);
		for(std::vector<ExpressionPtr>::iterator it = addedElements.begin() ; it != addedElements.end() ; ++it){
			ExpressionPtr addedElement = *it;
			if(!addedElement->getName().empty() && addedElement->subscript != NO_SUBSCRIPT)
				getUsingInstructions(addedElement->getName(), addedElement->subscript)->push_back(usingInstruction);
		}
		//addExpressionsToUsingInstruction(addedElements, usingInstruction);
		//delete addedElements;

		std::vector<ExpressionPtr> replacedElements;
		createExpressionElements(conditionPtr, true, replacedElements);//conditionPtr->getExpressionElements(true, replacedElements);
		for(std::vector<ExpressionPtr>::iterator it = replacedElements.begin() ; it != replacedElements.end() ; ++it){
			removeInstructionFromVariableUsesMap((*it)->getName(), (*it)->subscript, usingInstruction);
		}
		//removeExpressionsFromUsingInstruction(replacedElements, usingInstruction);
		//delete replacedElements;

		//delete *conditionPtr;
		conditionPtr = highLevelCondition;
	}
}

std::string ConditionCodePropagation::getComparisonOperand(BranchType branchType, FlagMacroType flagMacroType){
	switch(branchType){
	case Above:						//CF=0 and ZF=0
		if(flagMacroType == SUBFLAGS || flagMacroType == FCOMFLAGS){
			return ">";
		}
	case Above_or_Equal:			//CF=0
		if(flagMacroType == SUBFLAGS || flagMacroType == FCOMFLAGS){
			return ">=";
		}
		else if(flagMacroType == NEGFLAGS){
			return "==";
		}
		break;
	case Below:						//CF=1
		if(flagMacroType == SUBFLAGS || flagMacroType == FCOMFLAGS){
			return "<";
		}
		else if(flagMacroType == NEGFLAGS){
			return "!=";
		}
		break;
	case Below_or_Equal:			//CF=1 or ZF=1
		if(flagMacroType == SUBFLAGS || flagMacroType == FCOMFLAGS){
			return "<=";
		}
		else if(flagMacroType == LOGICALFLAGS)
			return "==";
		break;
	//case CX_Zero:					//CX=0
	case Equal:						//ZF=1
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS || flagMacroType == FCOMFLAGS || flagMacroType == ADDFLAGS){
			return "==";
		}
		break;
	case Greater:					//ZF=0 and SF=OF	(signed)
		if(flagMacroType == SUBFLAGS){
			return ">";
		}
		break;
	case Greater_or_Equal:			//SF=OF				(signed)
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS){
			return ">=";
		}
		break;
	case Less:						//SF != OF			(signed)
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS){
			return "<";
		}
		break;
	case Less_or_Equal:				//ZF=1 or SF != OF	(signed)
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS){
			return "<=";
		}
		break;
	case Not_Equal:					//ZF=0
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS || flagMacroType == ADDFLAGS){
			return "!=";
		}
		break;
	case Not_Overflow:				//OF=0				(signed)
		break;
	case Not_Parity:				//PF=0
		break;
	case Not_Signed:				//SF=0				(signed)
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS){
			return ">=";
		}
		break;
	case Overflow:					//OF=1				(signed)
		break;
	case Parity:					//PF=1
		break;
	case Signed:
		if(flagMacroType == SUBFLAGS || flagMacroType == LOGICALFLAGS){
			return "<";
		}
		break;
	}
	msg("ConditionCodePropagation::getComparisonOperand(%d, %d): branch type unhandled.\n", branchType, flagMacroType);
	return "COMPARISON_ERROR";
}

bool ConditionCodePropagation::sameDefiningInstruction(const FlagVectorPtr& testedFlags){
	if(testedFlags->size() == 1){
		return true;
	}
	else{
		FlagPtr flag = *(testedFlags->begin());
		InstructionPtr definingInstruction = getDefiningInstruction(flag->getName(), flag->subscript);//(*definitionsMap)[flag->getName()][flag->subscript];
		for(std::vector<FlagPtr>::iterator flag_iter = testedFlags->begin() + 1 ; flag_iter != testedFlags->end() ; ++flag_iter){
			flag = *flag_iter;
			InstructionPtr currentDefiningInstruction = getDefiningInstruction(flag->getName(), flag->subscript);
			if(definingInstruction != currentDefiningInstruction/*(*definitionsMap)[flag->getName()][flag->subscript]*/){
				return false;
			}
		}
	}
	return true;
}

bool ConditionCodePropagation::conditionsSetUsingPhiFunction(const FlagVectorPtr& testedFlags){
	std::vector<Phi_FunctionPtr> phi_flags_defs;
	std::vector<ExpressionPtr> phi_flags, macro_flags; 
	for(std::vector<FlagPtr>::iterator flag_iter = testedFlags->begin(); flag_iter != testedFlags->end() ; ++flag_iter){
		FlagPtr flag = *flag_iter;
		InstructionPtr definingInstruction = getDefiningInstruction(flag->getName(), flag->subscript);
		if(definingInstruction.get() == NULL || definingInstruction->type != PHI_FUNCTION)
			return false;
		Phi_FunctionPtr def_phi = std::dynamic_pointer_cast<Phi_Function>(definingInstruction);
		phi_flags_defs.push_back(def_phi);
		phi_flags.insert(phi_flags.end(), def_phi->arguments->begin(), def_phi->arguments->end());
	}

	Phi_FunctionPtr cond_phi = phi_flags_defs.front();
	for(std::vector<ExpressionPtr>::iterator it = cond_phi->arguments->begin() ; it != cond_phi->arguments->end() ; ++it){
		InstructionPtr def_inst = getDefiningInstruction((*it)->getName(), (*it)->subscript);
		if(def_inst.get() == NULL || def_inst->type != FLAG_MACRO)
			return false;
		FlagMacroPtr def_macro = std::dynamic_pointer_cast<FlagMacro>(def_inst);
		macro_flags.insert(macro_flags.end(), def_macro->definedFlags->begin(), def_macro->definedFlags->end());
	}

	for(std::vector<ExpressionPtr>::iterator it = phi_flags.begin() ; it != phi_flags.end() ; ++it){
		if(!isInExpressionList(macro_flags, *it))
			return false;
	}
	return true;
}

HighLevelConditionPtr ConditionCodePropagation::translateCondition(const LowLevelConditionPtr& lowLevelCondition){
	HighLevelConditionPtr highLevelCondition = getFloatingPointHighLevelCondition(lowLevelCondition);
	return !isExpressionPointerNull(highLevelCondition) ? highLevelCondition : getEFLAGSHighLevelCondition(lowLevelCondition);
}


HighLevelConditionPtr ConditionCodePropagation::getEFLAGSHighLevelCondition(const LowLevelConditionPtr& lowLevelCondition){
	HighLevelConditionPtr resultingHLCond = std::shared_ptr<HighLevelCondition>();
	//msg("low level condition: %s\n", lowLevelCondition->getExpressionString().c_str());
	if(conditionsSetUsingPhiFunction(lowLevelCondition->testedFlags)){
		ExpressionPtr flag = lowLevelCondition->testedFlags->front();
		Phi_FunctionPtr flag_phi_def = std::dynamic_pointer_cast<Phi_Function>(getDefiningInstruction(flag->getName(), flag->subscript));
		resultingHLCond = getHighLevelConditionFromPhiSet(lowLevelCondition, flag_phi_def);	
	}
	else if(sameDefiningInstruction(lowLevelCondition->testedFlags)){
		FlagPtr flag = *(lowLevelCondition->testedFlags->begin());
		InstructionPtr definingInstruction = getDefiningInstruction(flag->getName(), flag->subscript);//(*definitionsMap)[flag->getName()][flag->subscript];
		//msg("unique definition: %s\n", definingInstruction->getInstructionString().c_str());
		if(definingInstruction->type == FLAG_MACRO)
			resultingHLCond = getHighLevelConditionFromFlagMacro(lowLevelCondition, std::dynamic_pointer_cast<FlagMacro>(definingInstruction));
		else if (definingInstruction->type == PHI_FUNCTION){
			resultingHLCond = getHighLevelConditionFromLowLevelCondition(lowLevelCondition);
		}
		//return definingInstruction->type == FLAG_MACRO ? getHighLevelConditionFromFlagMacro(lowLevelCondition, (FlagMacro*)definingInstruction) : NULL;
	}
	return resultingHLCond;
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromPhiSet(const LowLevelConditionPtr& lowLevelCondition, const Phi_FunctionPtr& flag_phi_def){
	ExpressionVectorPtr phi_args = std::make_shared<std::vector<ExpressionPtr>>();
	for(std::vector<ExpressionPtr>::iterator flag_iter = flag_phi_def->arguments->begin() ; flag_iter != flag_phi_def->arguments->end(); ++flag_iter){
		ExpressionPtr flag = *flag_iter;
		FlagMacroPtr def_macro = std::dynamic_pointer_cast<FlagMacro>(getDefiningInstruction(flag->getName(), flag->subscript));
		HighLevelConditionPtr hlc = getHighLevelConditionFromFlagMacro(lowLevelCondition, def_macro);
		LocalVariablePtr cond = std::make_shared<LocalVariable>(flag->getName() + "_added_as_cond");
		cond->subscript = flag->subscript;
		cond->phiSourceBlockId = flag->phiSourceBlockId;
		phi_args->push_back(cond->deepcopy());
		AssignmentPtr cond_ass = std::make_shared<Assignment>(cond, hlc);
		updateDefinitionsMap(cond, cond_ass);
		std::vector<ExpressionPtr> usedElements;
		cond_ass->getUsedElements(usedElements);
		for(std::vector<ExpressionPtr>::iterator it = usedElements.begin() ; it != usedElements.end() ; ++it){
			ExpressionPtr usedElement = *it;
			if(!usedElement->getName().empty() && usedElement->subscript != NO_SUBSCRIPT)
				getUsingInstructions(usedElement->getName(), usedElement->subscript)->push_back(cond_ass);
		}
		controlFlowGraph->nodes->operator[](flag->phiSourceBlockId)->addAfterInstruction(cond_ass, def_macro);
	}
	LocalVariablePtr cond = std::make_shared<LocalVariable>(flag_phi_def->target->getName() + "_added_as_cond");
	cond->subscript = flag_phi_def->target->subscript;
	cond->phiSourceBlockId = flag_phi_def->target->phiSourceBlockId;
	Phi_FunctionPtr cond_phi = std::make_shared<Phi_Function>(cond, 0);
	cond_phi->arguments = phi_args;
	
	updateDefinitionsMap(cond, cond_phi);
	std::vector<ExpressionPtr> usedElements;
	cond_phi->getUsedElements(usedElements);
	for(std::vector<ExpressionPtr>::iterator it = usedElements.begin() ; it != usedElements.end() ; ++it){
		ExpressionPtr usedElement = *it;
		if(!usedElement->getName().empty() && usedElement->subscript != NO_SUBSCRIPT)
			getUsingInstructions(usedElement->getName(), usedElement->subscript)->push_back(cond_phi);
	}
	
	controlFlowGraph->nodes->operator[](flag_phi_def->target->phiSourceBlockId)->addAfterInstruction(cond_phi, flag_phi_def);
	return std::make_shared<HighLevelCondition>("==", cond_phi->target->deepcopy(), std::make_shared<NumericConstant>(0));
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromLowLevelCondition(const LowLevelConditionPtr& lowLevelCondition){
	HighLevelConditionPtr hlcond;
	if(lowLevelCondition->branchType == Equal){
		hlcond = std::make_shared<HighLevelCondition>("==", lowLevelCondition->testedFlags->front()->deepcopy(), std::make_shared<NumericConstant>(0));
	}
	else if(lowLevelCondition->branchType == Not_Equal){
		hlcond = std::make_shared<HighLevelCondition>("!=", lowLevelCondition->testedFlags->front()->deepcopy(), std::make_shared<NumericConstant>(0));
	}
	return hlcond;
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const FlagMacroPtr& definingFlagMacro){
	switch(definingFlagMacro->flagMacroType){
	case SUBFLAGS:
		return getHighLevelConditionFromSUBFlagMacro(lowLevelCondition, std::dynamic_pointer_cast<SUBFlagMacro>(definingFlagMacro));
	case LOGICALFLAGS:
		return getHighLevelConditionFromLogicalFlagMacro(lowLevelCondition, std::dynamic_pointer_cast<LogicalFlagMacro>(definingFlagMacro));
	case NEGFLAGS:
		return getHighLevelConditionFromNEGFlagMacro(lowLevelCondition, std::dynamic_pointer_cast<NEGFlagMacro>(definingFlagMacro));
	case FCOMFLAGS:
		return getHighLevelConditionFromFCOMFlagMacro(lowLevelCondition, std::dynamic_pointer_cast<FCOMFlagMacro>(definingFlagMacro));
	case ADDFLAGS:
		return getHighLevelConditionFromADDFlagMacro(lowLevelCondition, std::dynamic_pointer_cast<ADDFlagMacro>(definingFlagMacro));
	default:
		return std::shared_ptr<HighLevelCondition>();
	}
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromSUBFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const SUBFlagMacroPtr& definingSUBFlagMacro){
	HighLevelConditionPtr resultingHLCond;
	std::string comparisonOperand = getComparisonOperand(lowLevelCondition->branchType, SUBFLAGS);

	if(lowLevelCondition->branchType == Below && !isExpressionPointerNull(definingSUBFlagMacro->firstOperand) && !isExpressionPointerNull(definingSUBFlagMacro->secondOperand))
		return std::make_shared<HighLevelCondition>(comparisonOperand, 
													definingSUBFlagMacro->firstOperand->deepcopy(), 
													definingSUBFlagMacro->secondOperand->deepcopy(), 
													lowLevelCondition->isUnsignedCompare());

	if(!isExpressionPointerNull(definingSUBFlagMacro->result)){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, 
															   definingSUBFlagMacro->result->deepcopy(), 
															   zero, 
															   lowLevelCondition->isUnsignedCompare());
	}
	else{
		resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, 
															   definingSUBFlagMacro->firstOperand->deepcopy(), 
															   definingSUBFlagMacro->secondOperand->deepcopy(),
															   lowLevelCondition->isUnsignedCompare());
	}

	/*if(!isExpressionPointerNull(definingSUBFlagMacro->firstOperand)){
		resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingSUBFlagMacro->firstOperand->deepcopy(), definingSUBFlagMacro->secondOperand->deepcopy());
	}
	else{
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingSUBFlagMacro->result->deepcopy(), zero);
	}*/
	return resultingHLCond;
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromLogicalFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const LogicalFlagMacroPtr& definingLogicalFlagMacro){
	HighLevelConditionPtr resultingHLCond;
	if(onlyTest_OF_CF(lowLevelCondition->testedFlags)){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);

		resultingHLCond = std::make_shared<HighLevelCondition>("!=", zero, zero->deepcopy());
	}
	else{
		std::string comparisonOperand = getComparisonOperand(lowLevelCondition->branchType, LOGICALFLAGS);
		if(!isExpressionPointerNull(definingLogicalFlagMacro->result)){
			NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
			resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingLogicalFlagMacro->result->deepcopy(), zero);
		}
		else{
			resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingLogicalFlagMacro->firstOperand->deepcopy(), definingLogicalFlagMacro->secondOperand->deepcopy());
		}
	}
	return resultingHLCond;
}


HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromNEGFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const NEGFlagMacroPtr& definingNEGFlagMacro){
	std::string comparisonOperand = getComparisonOperand(lowLevelCondition->branchType, NEGFLAGS);
	NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
	HighLevelConditionPtr resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingNEGFlagMacro->operand->deepcopy(), zero);
	return resultingHLCond;
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromFCOMFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const FCOMFlagMacroPtr& definingFCOMFlagMacro){
	std::string comparisonOperand = getComparisonOperand(lowLevelCondition->branchType, FCOMFLAGS);
	HighLevelConditionPtr resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingFCOMFlagMacro->firstOperand->deepcopy(), definingFCOMFlagMacro->secondOperand->deepcopy());
	return resultingHLCond;
}

HighLevelConditionPtr ConditionCodePropagation::getHighLevelConditionFromADDFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const ADDFlagMacroPtr& definingADDFlagMacro){
	HighLevelConditionPtr resultingHLCond;
	std::string comparisonOperand = getComparisonOperand(lowLevelCondition->branchType, ADDFLAGS);
	if(!isExpressionPointerNull(definingADDFlagMacro->result)){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand, definingADDFlagMacro->result->deepcopy(), zero);
	}
	else{
		resultingHLCond = std::shared_ptr<HighLevelCondition>();
	}
	return resultingHLCond;
}

HighLevelConditionPtr ConditionCodePropagation::getFloatingPointHighLevelCondition(const LowLevelConditionPtr& lowLevelCondition){
	FlagPtr testedFlag = *(lowLevelCondition->testedFlags->begin());
	InstructionPtr definingInstruction = getDefiningInstruction(testedFlag->getName(), testedFlag->subscript);//(*definitionsMap)[testedFlag->getName()][testedFlag->subscript];

	HighLevelConditionPtr highLevelCondition = getFloatingPointHighLevelCondition_SAHF(lowLevelCondition, definingInstruction);
	return !isExpressionPointerNull(highLevelCondition) ? highLevelCondition : getFloatingPointHighLevelCondition_BitMask(lowLevelCondition, definingInstruction);
}

HighLevelConditionPtr ConditionCodePropagation::getFloatingPointHighLevelCondition_SAHF(const LowLevelConditionPtr& lowLevelCondition, const InstructionPtr& definingInstruction){
	HighLevelConditionPtr resultingHLCond = std::shared_ptr<HighLevelCondition>();
	FlagPtr fpuFlag = std::shared_ptr<Flag>();
	if(isConditionDefinedBySAHF(definingInstruction, fpuFlag)){
		InstructionPtr originalDefiningInstruction = getDefiningInstruction(fpuFlag->getName(), fpuFlag->subscript);//(*definitionsMap)[fpuFlag->getName()][fpuFlag->subscript];
		if(originalDefiningInstruction->type == FLAG_MACRO && std::dynamic_pointer_cast<FlagMacro>(originalDefiningInstruction)->flagMacroType == FCOMFLAGS){
			FCOMFlagMacroPtr originalDefiningFCOMFlagMacro = std::dynamic_pointer_cast<FCOMFlagMacro>(originalDefiningInstruction);
			std::string comparisonOperand = getComparisonOperand(lowLevelCondition->branchType, FCOMFLAGS);
			resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand,
																						 originalDefiningFCOMFlagMacro->firstOperand->deepcopy(),
																						 originalDefiningFCOMFlagMacro->secondOperand->deepcopy());
		}
	}
	return resultingHLCond;
}

bool ConditionCodePropagation::isConditionDefinedBySAHF(const InstructionPtr& definingInstruction, FlagPtr& fpuFlagPtr_out){
	InstructionPtr ahDefiningInstruction = std::shared_ptr<Instruction>();
	return isAssignmentToAH(definingInstruction, ahDefiningInstruction) &&
			isAHDefinedByFloatingPointFlags(ahDefiningInstruction, fpuFlagPtr_out);
}

bool ConditionCodePropagation::isAssignmentToAH(const InstructionPtr& usingInstruction, InstructionPtr& ahDefiningInstructionPtr_out){
	if(usingInstruction->type == ASSIGNMENT){
		ExpressionPtr rhsOperand = std::dynamic_pointer_cast<Assignment>(usingInstruction)->rhsOperand;
		if(rhsOperand->type == REGISTER && std::dynamic_pointer_cast<Register>(rhsOperand)->regNo == R_ah){
			ahDefiningInstructionPtr_out = getDefiningInstruction(rhsOperand->getName(), rhsOperand->subscript);
			//*ahDefiningInstructionPtr_out = (*definitionsMap)[rhsOperand->getName()][rhsOperand->subscript];
			return true;
		}
	}
	return false;
}

bool ConditionCodePropagation::isAHDefinedByFloatingPointFlags(const InstructionPtr& ahDefiningInstruction, FlagPtr& fpuFlagPtr_out){
	if(ahDefiningInstruction->type == ASSIGNMENT){
		ExpressionPtr rhsOperandAH = std::dynamic_pointer_cast<Assignment>(ahDefiningInstruction)->rhsOperand;
		if(rhsOperandAH->type == BIT_CONCATENATION){
			intToFlagMapPtr flagMap = std::dynamic_pointer_cast<FlagConcatenation>(rhsOperandAH)->flagMap;
			for(std::map<int, FlagPtr>::iterator flag_iter = flagMap->begin() ; flag_iter != flagMap->end() ; ++flag_iter){
				if(flag_iter->second->flagType != FPU_FLAG){
					return false;
				}
			}

			fpuFlagPtr_out = flagMap->begin()->second;
			return true;
		}
	}
	return false;
}

HighLevelConditionPtr ConditionCodePropagation::getFloatingPointHighLevelCondition_BitMask(const LowLevelConditionPtr& lowLevelCondition, const InstructionPtr& definingInstruction){
	HighLevelConditionPtr resultingHLCond = std::shared_ptr<HighLevelCondition>();
	FlagPtr fpuFlag = std::shared_ptr<Flag>();
	int bitMask;
	if(isConditionDefinedByBitMask(definingInstruction, fpuFlag, bitMask)){
		InstructionPtr flagDefiningInstruction = getDefiningInstruction(fpuFlag->getName(), fpuFlag->subscript);//(*definitionsMap)[fpuFlag->getName()][fpuFlag->subscript];
		if(flagDefiningInstruction->type == FLAG_MACRO && std::dynamic_pointer_cast<FlagMacro>(flagDefiningInstruction)->flagMacroType == FCOMFLAGS){
			FCOMFlagMacroPtr flagDefiningFCOMFlagMacro = std::dynamic_pointer_cast<FCOMFlagMacro>(flagDefiningInstruction);
			std::string comparisonOperand = getFloatingPointComparisonOperand(lowLevelCondition->branchType, bitMask);
			resultingHLCond = std::make_shared<HighLevelCondition>(comparisonOperand,
																   flagDefiningFCOMFlagMacro->firstOperand->deepcopy(),
																   flagDefiningFCOMFlagMacro->secondOperand->deepcopy());
		}
	}
	return resultingHLCond;
}

bool ConditionCodePropagation::isConditionDefinedByBitMask(const InstructionPtr& conditionDefiningInstruction, FlagPtr& fpuFlagPtr_out, int &bitMaskPtr){
	InstructionPtr ahDefiningInstruction = std::shared_ptr<Instruction>();
	return doesLogicalFlagMacroCompare_AH_Constant(conditionDefiningInstruction, ahDefiningInstruction, bitMaskPtr) &&
			isAHDefinedByFloatingPointFlags(ahDefiningInstruction, fpuFlagPtr_out);
}

bool ConditionCodePropagation::doesLogicalFlagMacroCompare_AH_Constant(const InstructionPtr& conditionDefiningInstruction, InstructionPtr& ahDefiningInstructionPtr_out, int &bitMaskPtr){
	if(conditionDefiningInstruction->type == FLAG_MACRO){
		FlagMacroPtr definingFlagMacro = std::dynamic_pointer_cast<FlagMacro>(conditionDefiningInstruction);
		if(definingFlagMacro->flagMacroType == LOGICALFLAGS){
			LogicalFlagMacroPtr definingLogicalFlagMacro = std::dynamic_pointer_cast<LogicalFlagMacro>(definingFlagMacro);
			if(doesCompareRegisterWithConstant(definingLogicalFlagMacro)){
				bitMaskPtr = (int)(std::dynamic_pointer_cast<NumericConstant>(definingLogicalFlagMacro->secondOperand)->value);
				RegisterPtr testedRegister = std::dynamic_pointer_cast<Register>(definingLogicalFlagMacro->firstOperand);
				if(testedRegister->regNo == R_ah){
					ahDefiningInstructionPtr_out = getDefiningInstruction(testedRegister->getName(), testedRegister->subscript);//(*definitionsMap)[testedRegister->getName()][testedRegister->subscript];
					return true;
				}
			}
		}
	}
	return false;
}

bool ConditionCodePropagation::doesCompareRegisterWithConstant(const LogicalFlagMacroPtr& logicalFlagMacro){
	return logicalFlagMacro->firstOperand != NULL &&
		   logicalFlagMacro->firstOperand->type == REGISTER &&
		   logicalFlagMacro->secondOperand != NULL &&
		   logicalFlagMacro->secondOperand->type == NUMERIC_CONSTANT;
}

std::string ConditionCodePropagation::getFloatingPointComparisonOperand(BranchType branchType, int bitMask){
	switch(bitMask){
	case 0x41://C3,C0
		switch(branchType){
		case Equal:
		case Parity:
			return ">";
		case Not_Equal:
		case Not_Parity:
			return "<=";
		}
	case 0x05://C2,C0
	case 0x01://C0
		switch(branchType){
		case Equal:
		case Parity:
			return ">=";
		case Not_Equal:
		case Not_Parity:
			return "<";
		}
	case 0x40://C2
	case 0x44://C3,C2
		switch(branchType){
		case Equal:
		case Parity:
			return "!=";
		case Not_Equal:
		case Not_Parity:
			return "==";
		}
	}
	msg("ConditionCodePropagation::getFloatingPointComparisonOperand() ERROR\n");
	return "ERROR";
}

bool ConditionCodePropagation::onlyTest_OF_CF(FlagVectorPtr testedFlags){
	for(std::vector<FlagPtr>::iterator flag_iter = testedFlags->begin() ; flag_iter != testedFlags->end() ; ++flag_iter){
		FlagNo testedFlagNo = (*flag_iter)->flagNo;
		if(testedFlagNo != CF && testedFlagNo != OF){
			return false;
		}
	}
	return true;
}

HighLevelConditionPtr ConditionCodePropagation::getOpaquePredicateFromLogicalInstruction(FlagNo testedFlag, BranchType branchType){
	HighLevelConditionPtr opaquePredicate;
	std::string comparisonOperator = "UnknownComparisonOperator";
	switch(testedFlag){
	case CF:
		if(branchType == Above_or_Equal)
			comparisonOperator = "==";
		else if(branchType == Below)
			comparisonOperator = "!=";
		break;
	case OF:
		if(branchType == Not_Overflow)
			comparisonOperator = "==";
		else if(branchType == Overflow)
			comparisonOperator = "!=";
		break;
	}
	NumericConstantPtr zero1 = std::make_shared<NumericConstant>(0);
	NumericConstantPtr zero2 = std::make_shared<NumericConstant>(0);
	opaquePredicate = std::make_shared<HighLevelCondition>(comparisonOperator, zero1, zero2);
	return opaquePredicate;
}

//HighLevelConditionPtr ConditionCodePropagation::getOpaquePredicateFromLogicalInstruction_CF(BranchType branchType){
//	//if((flagStatus))
//}
