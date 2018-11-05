/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "DataFlowAlgorithm.h"
//#include "../../IntermediateRepresentation/IRSharedPointers.h"

class ConditionCodePropagation :
	public DataFlowAlgorithm
{
public:
	ConditionCodePropagation(ControlFlowGraphPtr _controlFlowGraph,
							 definitionsMapPtr _definitionsMap,
							 definitionsMapPtr _flagDefinitionsMap,
							 usesMapPtr _usesMap);
	virtual ~ConditionCodePropagation(void);

	void applyAlgorithm();
	void replace_remaining_flag_macros();

private:
	
	void replaceFlagUsesByEquivalentConditions(const std::string& flagName);
	void replaceLowLevelConditionInConditionalJump(ConditionalJumpPtr usingConditionalJump);
	void replaceLowLevelConditionInAssignment(AssignmentPtr usingAssignment);
	void replaceLowLevelCondition(ExpressionPtr& conditionPtr, InstructionPtr usingInstruction);

	std::string getComparisonOperand(BranchType branchType, FlagMacroType flagMacroType);
	HighLevelConditionPtr translateCondition(const LowLevelConditionPtr& lowLevelCondition);

	HighLevelConditionPtr getEFLAGSHighLevelCondition(const LowLevelConditionPtr& lowLevelCondition);
	HighLevelConditionPtr getHighLevelConditionFromFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const FlagMacroPtr& definingFlagMacro);
	HighLevelConditionPtr getHighLevelConditionFromLowLevelCondition(const LowLevelConditionPtr& lowLevelCondition);
	HighLevelConditionPtr getHighLevelConditionFromSUBFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const SUBFlagMacroPtr& definingSUBFlagMacro);
	HighLevelConditionPtr getHighLevelConditionFromLogicalFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const LogicalFlagMacroPtr& definingLogicalFlagMacro);
	HighLevelConditionPtr getHighLevelConditionFromNEGFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const NEGFlagMacroPtr& definingNEGFlagMacro);
	HighLevelConditionPtr getHighLevelConditionFromFCOMFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const FCOMFlagMacroPtr& definingFCOMFlagMacro);
	HighLevelConditionPtr getHighLevelConditionFromADDFlagMacro(const LowLevelConditionPtr& lowLevelCondition, const ADDFlagMacroPtr& definingADDFlagMacro);
	HighLevelConditionPtr getHighLevelConditionFromPhiSet(const LowLevelConditionPtr& lowLevelCondition, const Phi_FunctionPtr& flag_phi_def);

	
	HighLevelConditionPtr getFloatingPointHighLevelCondition(const LowLevelConditionPtr& lowLevelCondition);
	
	HighLevelConditionPtr getFloatingPointHighLevelCondition_SAHF(const LowLevelConditionPtr& lowLevelCondition, const InstructionPtr& definingInstruction);
	bool isConditionDefinedBySAHF(const InstructionPtr& definingInstruction, FlagPtr& fpuFlagPtr_out);
	bool isAssignmentToAH(const InstructionPtr& usingInstruction, InstructionPtr& ahDefiningInstructionPtr_out);
	
	
	HighLevelConditionPtr getFloatingPointHighLevelCondition_BitMask(const LowLevelConditionPtr& lowLevelCondition, const InstructionPtr& definingInstruction);
	bool isConditionDefinedByBitMask(const InstructionPtr& conditionDefiningInstruction, FlagPtr& fpuFlagPtr_out, int &bitMaskPtr);
	bool doesLogicalFlagMacroCompare_AH_Constant(const InstructionPtr& conditionDefiningInstruction, InstructionPtr& ahDefiningInstructionPtr_out, int &bitMaskPtr);
	bool doesCompareRegisterWithConstant(const LogicalFlagMacroPtr& logicalFlagMacro);

	bool isAHDefinedByFloatingPointFlags(const InstructionPtr& ahDefiningInstruction, FlagPtr& fpuFlagPtr_out);

	
	std::string getFloatingPointComparisonOperand(BranchType branchType, int bitMask);
	bool sameDefiningInstruction(const FlagVectorPtr& testedFlags);
	bool conditionsSetUsingPhiFunction(const FlagVectorPtr& testedFlags);

	bool onlyTest_OF_CF(FlagVectorPtr testedFlags);
	HighLevelConditionPtr getOpaquePredicateFromLogicalInstruction(FlagNo testedFlag, BranchType branchType);
	//HighLevelConditionPtr getOpaquePredicateFromLogicalInstruction_CF(BranchType branchType);
};

