/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <algorithm>
#include <map>

#include <ida.hpp>
#include <idp.hpp>
#include <gdl.hpp>

#include "OperandHandler.h"
#include "MathematicalOperators.h"
#include "../IRSharedPointers.h"
#include "../../TypeAnalysis/TypeSharedPointers.h"

class Instruction;
class LocalVariable;
class Register;

#define FPU_Registers_Num 8

class InstructionHandler
{
public:
	InstructionHandler(flowChartPtr _flow_chart);
	virtual ~InstructionHandler(void);

	void updateInstructionList(InstructionVectorPtr& inst_list);

private:
	OperandHandler opHandler;
	flowChartPtr flow_chart;
	int word_size;
	int tmpSubscript;
	//int maxFPUIndex;

	int getNodeID(ea_t addr);
	// bool get_func_type(ea_t addr, func_type_info_t* fi, std::string &declarationString);
	bool get_func_type(ea_t addr, func_type_data_t *fi);
	// void decodeType(const qtype &typeString);
	RegNo getFPRegByIndex(int fpreg_index);

	//void insertBinaryInstruction(std::string operation, std::vector<Instruction*>* inst_list, bool isCommutative);
	void insertAssignmentWithAssociativeCommutativeExpression(MathematicalOperator mathematicalOperator, InstructionVectorPtr& inst_list);
	void insertAssignmentWithBinaryNonCommutativeExpression(MathematicalOperator mathematicalOperator, InstructionVectorPtr& inst_list);
	ExpressionPtr getSubtractionExpression(ExpressionPtr firstOperand, ExpressionPtr secondOperand);
	ExpressionPtr getDivisionExpression(ExpressionPtr divident, ExpressionPtr divisor);
	void insertFPUStackAssignments(InstructionVectorPtr& inst_list, bool isPush);

	void handle_ADD(InstructionVectorPtr& inst_list);
	void handle_AND(InstructionVectorPtr& inst_list);
	void handle_CALL(InstructionVectorPtr& inst_list);
	void handle_CMP(InstructionVectorPtr& inst_list);
	void handle_CMPS(InstructionVectorPtr& inst_list);
	void handle_ConditionalJump(ExpressionPtr lowLevelCondition, InstructionVectorPtr& inst_list);
	void handle_JCXZ(char reg_dtyp, InstructionVectorPtr& inst_list);
	void handle_JMP(InstructionVectorPtr& inst_list);
	void handle_DEC(InstructionVectorPtr& inst_list);
	void handle_DIV(InstructionVectorPtr& inst_list);


	void handle_F2XM1(InstructionVectorPtr& inst_list);
	void handle_FABS(InstructionVectorPtr& inst_list);
	void handle_FADD(InstructionVectorPtr& inst_list);
	void handle_FADDP(InstructionVectorPtr& inst_list);
	void handle_FCHS(InstructionVectorPtr& inst_list);
	void handle_FCOM(InstructionVectorPtr& inst_list, bool setEFLAGS, int numPops);
	void handle_FCOS(InstructionVectorPtr& inst_list);
	void handle_FDIV(InstructionVectorPtr& inst_list, bool isReverse);
	void handle_FDIVP(InstructionVectorPtr& inst_list, bool isReverse);
	void handle_FISTTP(InstructionVectorPtr& inst_list);
	void handle_FLD(InstructionVectorPtr& inst_list);
	void handle_FLDConstant(ExpressionPtr floatingPointConstant, InstructionVectorPtr& inst_list);
	void handle_FMUL(InstructionVectorPtr& inst_list);
	void handle_FMULP(InstructionVectorPtr& inst_list);
	void handle_FPATAN(InstructionVectorPtr& inst_list);
	void handle_FPTAN(InstructionVectorPtr& inst_list);
	void handle_FRNDINT(InstructionVectorPtr& inst_list);
	void handle_FSIN(InstructionVectorPtr& inst_list);
	void handle_FSINCOS(InstructionVectorPtr& inst_list);
	void handle_FST(InstructionVectorPtr& inst_list);
	void handle_FSTSW(InstructionVectorPtr& inst_list);
	void handle_FSTP(InstructionVectorPtr& inst_list);
	void handle_FSUB(InstructionVectorPtr& inst_list, bool isReverse);
	void handle_FSUBP(InstructionVectorPtr& inst_list, bool isReverse);
	void handle_FTST(InstructionVectorPtr& inst_list);
	void handle_FXCH(InstructionVectorPtr& inst_list);
	void handle_FYL2X(InstructionVectorPtr& inst_list);
	void handle_FYL2XP1(InstructionVectorPtr& inst_list);
	
	
	void handle_INC(InstructionVectorPtr& inst_list);
	void handle_IMUL(InstructionVectorPtr& inst_list);
	void handle_LOOP(InstructionVectorPtr& inst_list);
	void handle_MUL(InstructionVectorPtr& inst_list);
	void handle_MOV(InstructionVectorPtr& inst_list);
	void handle_MOVS(InstructionVectorPtr& inst_list);
	void handle_NEG(InstructionVectorPtr& inst_list);
	void handle_NOT(InstructionVectorPtr& inst_list);
	void handle_OR(InstructionVectorPtr& inst_list);
	void handle_POP(InstructionVectorPtr& inst_list);
	void handle_PUSH(InstructionVectorPtr& inst_list);
	void handle_RET(InstructionVectorPtr& inst_list);
	void handle_ROTATE(InstructionVectorPtr& inst_list, std::string rotFunctionName);
	void handle_SAHF(InstructionVectorPtr& inst_list);
	void handle_SAL(InstructionVectorPtr& inst_list);
	void handle_SAR(InstructionVectorPtr& inst_list);
	void handle_SBB(InstructionVectorPtr& inst_list);
	void handle_SCAS(InstructionVectorPtr& inst_list);
	void handle_SETCond(ExpressionPtr lowLevelCondition, InstructionVectorPtr& inst_list);
	void handle_SHR(InstructionVectorPtr& inst_list);
	void handle_STOS(InstructionVectorPtr& inst_list);
	void handle_SUB(InstructionVectorPtr& inst_list);
	void handle_TEST(InstructionVectorPtr& inst_list);
	void handle_XCHG(InstructionVectorPtr& inst_list);
	void handle_XOR(InstructionVectorPtr& inst_list);

	bool isREP_Prefix();
	bool isREPNE_Prefix();

	InstructionPtr getStringFunction(std::string functionName);
	ExpressionPtr getLengthForStringInstructions();
	ExpressionPtr getAddition(ExpressionPtr op1, ExpressionPtr op2);
	ExpressionPtr getIncDecExpression();

	bool constructSwitchInfo(switch_info_ex_t* si);
	nodeToCasesMapPtr getNodeToCasesMap(switch_info_ex_t* si);
	RegisterPtr getIndexingRegister();
	LocalVariablePtr getTmpVariable();
	LocalVariablePtr getFtmpVariable();

	AssignmentPtr getAssignment_floatingPoint(ExpressionPtr target, ExpressionPtr source);
	AssignmentPtr getAssignment_integer(ExpressionPtr target, ExpressionPtr source);
	TypePtr getFloatingPointType();
	IntegralTypePtr getUnsignedIntegerType(int operandNumber = 0);
	IntegralTypePtr getSignedIntegerType(int operandNumber = 0);
	PointerTypePtr getPointerType();

	void decrementStackPointer(InstructionVectorPtr& inst_list);
	void saveExpressionOnStack(InstructionVectorPtr& inst_list, ExpressionPtr expressionToSave);
};
