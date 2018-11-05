////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "InstructionHandler.h"

#include "IntermediateRepresentation/IRHeaders.h"
#include <math.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "Shared/InstructionUtilities.h"

#include "TypeAnalysis/TypeDecoder.h"
#include "TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "TypeAnalysis/Types/SimpleType/PointerType.h"

InstructionHandler::InstructionHandler(flowChartPtr _flow_chart)
{
	flow_chart = _flow_chart;
	word_size = 0;
	if(ad16()){
		word_size = 2;
	}
	else if(ad32()){
		word_size = 4;
	}
	else if(ad64()){
		word_size = 8;
	}
	tmpSubscript = 0;
	//maxFPUIndex = _maxFPUIndex;
}


InstructionHandler::~InstructionHandler(void)
{
	flow_chart = NULL;
}


void InstructionHandler::updateInstructionList(InstructionVectorPtr& inst_list){
	switch_info_ex_t si;
	if(constructSwitchInfo(&si)){
		ExpressionPtr targetBlock = opHandler.ExpressionFromOperand(0);
		RegisterPtr indexRegister = getIndexingRegister();
		int defaultBlockId = getNodeID(si.defjump);
		ExpressionPtr switchVariable = std::shared_ptr<Expression>();
		if(decode_insn(si.startea))
			switchVariable = opHandler.ExpressionFromOperand(0);
		IndirectTableJumpPtr jumpTable = std::make_shared<IndirectTableJump>(targetBlock, indexRegister, switchVariable, getNodeToCasesMap(&si), defaultBlockId);
		jumpTable->startAddressOfSwitchIdiom = si.startea;
		inst_list->push_back(jumpTable);
		//msg("\n<><><><><><><><><><>\n");
		//
		//msg("switch(%s) at %x\n", indexRegister->getExpressionString().c_str(), cmd.ea);
		//msg("instruction: %s\n", jumpTable->getInstructionString().c_str());
		//casevec_t cases;
		//eavec_t eas;
		//if(calc_switch_cases(cmd.ea, &si, &cases, &eas)){
		//	msg("%d cases\n%d eas\n", cases.size(), eas.size());
		//	for(int nCase = 0 ; nCase < cases.size() ; ++nCase){
		//		msg("\t%x: ", eas[nCase]);
		//		for(int i = 0 ; i < cases[nCase].size() ; ++i){
		//			msg("%d, ", cases[nCase][i]);
		//		}
		//		msg("\n");
		//	}
		//	msg("\n");
		//}
		//msg("/tdefault %d\n", defaultBlockId);
		////msg("/tdefault %d\n", defaultBlockId);
		//msg("<><><><><><><><><><>\n");
	}
	else{
		switch(cmd.itype){
		case NN_add:
			handle_ADD(inst_list);
			break;
		case NN_and:
			handle_AND(inst_list);
			break;
		case NN_call:
		case NN_callfi:
		case NN_callni:
			handle_CALL(inst_list);
			break;
		case NN_cmp:
			handle_CMP(inst_list);
			break;
		case NN_cmps:
			handle_CMPS(inst_list);
			break;
		case NN_cdq:
			 /*
			cdq
			idiv  ecx
			-> 
			edx = eax % ecx
			eax = eax / ecx
			This does not need to be handled since the next iteration will decode the idiv instruction*/
			break;
		case NN_dec:
			handle_DEC(inst_list);
			break;
		case NN_idiv:
		case NN_div:
			handle_DIV(inst_list);
			break;
		case NN_enter:
			break;
		case NN_f2xm1:
			handle_F2XM1(inst_list);
			break;
		case NN_fabs:
			handle_FABS(inst_list);
			break;
		case NN_fadd:
			handle_FADD(inst_list);
			break;
		case NN_faddp:
			handle_FADDP(inst_list);
			break;
		case NN_fchs:
			handle_FCHS(inst_list);
			break;
		case NN_fcos:
			handle_FCOS(inst_list);
			break;
		case NN_fucom:
		case NN_fcom:
			handle_FCOM(inst_list, false, 0);
			break;
		case NN_fucomi:
		case NN_fcomi:
			handle_FCOM(inst_list, true, 0);
			break;
		case NN_fucomp:
		case NN_fcomp:
			handle_FCOM(inst_list, false, 1);
			break;
		case NN_fucomip:
		case NN_fcomip:
			handle_FCOM(inst_list, true, 1);
			break;
		case NN_fucompp:
		case NN_fcompp:
			handle_FCOM(inst_list, false, 2);
			break;
		case NN_fidiv:
		case NN_fdiv:
			handle_FDIV(inst_list, false);
			break;
		case NN_fidivr:
		case NN_fdivr:
			handle_FDIV(inst_list, true);
			break;
		case NN_fdivp:
			handle_FDIVP(inst_list, false);
			break;
		case NN_fdivrp:
			handle_FDIVP(inst_list, true);
			break;
		case NN_fisttp:
			handle_FISTTP(inst_list);
			break;
		case NN_fbld:
		case NN_fild:
		case NN_fld:
			handle_FLD(inst_list);
			break;
		case NN_fld1:
			{
				NumericConstantPtr one = std::make_shared<NumericConstant>(1);
				handle_FLDConstant(one, inst_list);
			}
			
			//handle_FLDConstant(new NumericLiteral(1), inst_list);
			break;
		case NN_fldl2e:
			{
				SymbolicConstantPtr constVar = std::make_shared<SymbolicConstant>(E);
				MathFunctionPtr mathFnPtr = std::make_shared<MathFunction>(LOG_2, constVar);
				handle_FLDConstant(mathFnPtr, inst_list);
			}
			//handle_FLDConstant(new MathFunction(LOG_2, new NumericLiteral(E)), inst_list);
			break;
		case NN_fldl2t:
			{
				NumericConstantPtr constVar = std::make_shared<NumericConstant>(10);
				MathFunctionPtr mathFnPtr = std::make_shared<MathFunction>(LOG_2, constVar);
				handle_FLDConstant(mathFnPtr, inst_list);
			}
			//handle_FLDConstant(new MathFunction(LOG_2, new NumericLiteral(10)), inst_list);
			break;
		case NN_fldlg2:
			{
				NumericConstantPtr constVar = std::make_shared<NumericConstant>(2);
				MathFunctionPtr mathFnPtr = std::make_shared<MathFunction>(LOG_10, constVar);
				handle_FLDConstant(mathFnPtr, inst_list);
			}
			//handle_FLDConstant(new MathFunction(LOG_10, new NumericLiteral(2)), inst_list);
			break;
		case NN_fldln2:
			{
				NumericConstantPtr constVar = std::make_shared<NumericConstant>(2);
				MathFunctionPtr mathFnPtr = std::make_shared<MathFunction>(LN, constVar);
				handle_FLDConstant(mathFnPtr, inst_list);
			}
			//handle_FLDConstant(new MathFunction(LN, new NumericLiteral(2)), inst_list);
			break;
		case NN_fldpi:
			{
				SymbolicConstantPtr pi = std::make_shared<SymbolicConstant>(PI);
				handle_FLDConstant(pi, inst_list);
			}
			//handle_FLDConstant(new NumericLiteral(PI), inst_list);
			break;
		case NN_fldz:
			{
				NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
				handle_FLDConstant(zero, inst_list);
			}
			//handle_FLDConstant(new NumericLiteral(0), inst_list);
			break;
		case NN_fimul:
		case NN_fmul:
			handle_FMUL(inst_list);
			break;
		case NN_fmulp:
			handle_FMULP(inst_list);
			break;
		case NN_fpatan:
			handle_FPATAN(inst_list);
			break;
		case NN_fptan:
			handle_FPTAN(inst_list);
			break;
		case NN_frndint:
			handle_FRNDINT(inst_list);
			break;
		case NN_fsin:
			handle_FSIN(inst_list);
			break;
		case NN_fsincos:
			handle_FSINCOS(inst_list);
			break;
		case NN_fist:
		case NN_fst:
			handle_FST(inst_list);
			break;
		case NN_fstsw:
		case NN_fnstsw:
			handle_FSTSW(inst_list);
			break;
		case NN_fbstp:
		case NN_fistp:
		case NN_fstp:
			handle_FSTP(inst_list);
			break;
		case NN_fisub:
		case NN_fsub:
			handle_FSUB(inst_list, false);
			break;
		case NN_fsubp:
			handle_FSUBP(inst_list, false);
			break;
		case NN_fisubr:
		case NN_fsubr:
			handle_FSUB(inst_list, true);
			break;
		case NN_fsubrp:
			handle_FSUBP(inst_list, true);
			break;
		case NN_ftst:
			handle_FTST(inst_list);
			break;
		case NN_fxch:
			handle_FXCH(inst_list);
			break;
		case NN_fyl2x:
			handle_FYL2X(inst_list);
			break;
		case NN_fyl2xp1:
			handle_FYL2XP1(inst_list);
			break;
		case NN_inc:
			handle_INC(inst_list);
			break;
		case NN_ja:
		case NN_jnbe:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Above);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jae:
		case NN_jnb:
		case NN_jnc:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Above_or_Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jb:
		case NN_jc:
		case NN_jnae:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Below);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jbe:
		case NN_jna:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Below_or_Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jcxz:
			handle_JCXZ(dt_word, inst_list);
			break;
		case NN_jecxz:
			handle_JCXZ(dt_dword, inst_list);
			break;
		case NN_je:
		case NN_jz:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jg:
		case NN_jnle:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Greater);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jge:
		case NN_jnl:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Greater_or_Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jl:
		case NN_jnge:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Less);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jle:
		case NN_jng:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Less_or_Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jne:
		case NN_jnz:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jno:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Overflow);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jnp:
		case NN_jpo:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF);
				testedFlags->push_back(flagPF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Parity);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jns:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Signed);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jo:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Overflow);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jp:
		case NN_jpe:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF);
				testedFlags->push_back(flagPF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Parity);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_js:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Signed);
				handle_ConditionalJump(lowLevelCondition, inst_list);
			}
			break;
		case NN_jmp:
		case NN_jmpfi:
		case NN_jmpni:
			handle_JMP(inst_list);
			break;
		case NN_leave:
			break;
		case NN_loop:
			handle_LOOP(inst_list);
			break;
		case NN_lea:
		case NN_movsx:
		case NN_movzx:
		case NN_mov:
			handle_MOV(inst_list);
			break;
		case NN_movs:
			handle_MOVS(inst_list);
			break;
		case NN_imul:
			handle_IMUL(inst_list);
			break;
		case NN_mul:
			handle_MUL(inst_list);
			break;
		case NN_neg:
			handle_NEG(inst_list);
			break;
		case NN_not:
			handle_NOT(inst_list);
			break;
		case NN_or:
			handle_OR(inst_list);
			break;
		case NN_pop:
			handle_POP(inst_list);
			break;
		case NN_push:
			handle_PUSH(inst_list);
			break;
		case NN_retn:
		case NN_retf:
			handle_RET(inst_list);
			break;
		case NN_rcl:
			handle_ROTATE(inst_list, "RCL_");
			break;
		case NN_rcr:
			handle_ROTATE(inst_list, "RCR_");
			break;
		case NN_rol:
			handle_ROTATE(inst_list, "ROL_");
			break;
		case NN_ror:
			handle_ROTATE(inst_list, "ROR_");
			break;
		case NN_sahf:
			handle_SAHF(inst_list);
			break;
		case NN_sbb:
			handle_SBB(inst_list);
			break;
		case NN_scas:
			handle_SCAS(inst_list);
			break;
		case NN_sal:
		case NN_shl:
			handle_SAL(inst_list);
			break;
		case NN_sar:
		case NN_shr:
			handle_SAR(inst_list);
			break;
		case NN_seta:
		case NN_setnbe:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Above);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setae:
		case NN_setnc:
		case NN_setnb:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Above_or_Equal);
				handle_ConditionalJump(lowLevelCondition, inst_list);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setb:
		case NN_setc:
		case NN_setnae:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Below);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setbe:
		case NN_setna:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
				testedFlags->push_back(flagCF);
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Below_or_Equal);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_sete:
		case NN_setz:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Equal);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setne:
		case NN_setnz:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Equal);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setl:
		case NN_setnge:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Less);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setge:
		case NN_setnl:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Greater_or_Equal);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setle:
		case NN_setng:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Less_or_Equal);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setg:
		case NN_setnle:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
				testedFlags->push_back(flagZF);
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Greater);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_sets:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Signed);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setns:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
				testedFlags->push_back(flagSF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Signed);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_seto:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Overflow);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setno:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagOF = std::make_shared<Flag>(CPU_FLAG, OF);
				testedFlags->push_back(flagOF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Overflow);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setp:
		case NN_setpe:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF);
				testedFlags->push_back(flagPF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Parity);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		case NN_setnp:
		case NN_setpo:
			{
				FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
				FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF);
				testedFlags->push_back(flagPF);
				LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Not_Parity);
				handle_SETCond(lowLevelCondition, inst_list);
			}
			break;
		/*case NN_shr:
			handle_SHR(inst_list);
			break;*/
		case NN_stos:
			handle_STOS(inst_list);
			break;
		case NN_sub:
			handle_SUB(inst_list);
			break;
		case NN_test:
			handle_TEST(inst_list);
			break;
		case NN_xchg:
			handle_XCHG(inst_list);
			break;
		case NN_xor:
			handle_XOR(inst_list);
			break;
		case NN_nop:
			break;
		default:
			msg("Instruction at %x with itype: %d is not supported.\n", cmd.ea, cmd.itype);
		}
	}
}

bool InstructionHandler::constructSwitchInfo(switch_info_ex_t* si){
	return (get_flags_novalue(cmd.ea) & FF_JUMP) != 0
			&& get_switch_info_ex(cmd.ea, si, sizeof(*si)) > 0;
}

nodeToCasesMapPtr InstructionHandler::getNodeToCasesMap(switch_info_ex_t* si){
	nodeToCasesMapPtr nodeToCasesMap = std::make_shared<std::map<int, std::set<int>>>();
	
	casevec_t cases;
	eavec_t eas;
	if(calc_switch_cases(cmd.ea, si, &cases, &eas)){
		for(int nCase = 0 ; nCase < cases.size() ; ++nCase){
			int caseNodeId = getNodeID(eas[nCase]);
			for(svalvec_t::iterator caseVaueIter = cases[nCase].begin() ; caseVaueIter != cases[nCase].end() ; ++caseVaueIter){
				nodeToCasesMap->operator[](caseNodeId).insert(*caseVaueIter);
			}
		}
	}

	/*for(std::map<int, std::set<int>>::iterator it1 = nodeToCasesMap->begin() ; it1 != nodeToCasesMap->end() ; ++it1){
		msg("Node(%d) --> ", it1->first);
		for(std::set<int>::iterator it2 = it1->second.begin() ; it2 != it1->second.end() ; ++it2){
			msg("%d  ", *it2);
		}
		msg("\n");
	}*/

	return nodeToCasesMap;
}

RegisterPtr InstructionHandler::getIndexingRegister(){
	RegisterPtr regPtr = std::shared_ptr<Register>();
	op_t op = cmd.Operands[0];
	if(op.hasSIB){
		regPtr = std::make_shared<Register>(sib_index(op), opHandler.getAddressingRegisterDType());
	}
	return regPtr;
	//return op.hasSIB ? new Register(sib_index(op), opHandler.getAddressingRegisterDType()) : NULL;
}

int InstructionHandler::getNodeID(ea_t addr){
	qflow_chart_t::blocks_t nodes = flow_chart->blocks;
	for(unsigned int id = 0 ; id < nodes.size() ; id++){
		if(addr >= nodes[id].startEA && addr < nodes[id].endEA){
			return id;
		}
	}
	return -1;
}

bool InstructionHandler::get_func_type(ea_t addr, func_type_data_t *fi){
	tinfo_t tif;
	if(get_tinfo2(addr, &tif)){
		return tif.get_func_details(fi);
	}
	else if(guess_tinfo2(addr, &tif)){
		return tif.get_func_details(fi);
	}
	return false;
}


RegNo InstructionHandler::getFPRegByIndex(int fpreg_index){
	switch(fpreg_index){
	case 0:
		return R_st0;
	case 1:
		return R_st1;
	case 2:
		return R_st2;
	case 3:
		return R_st3;
	case 4:
		return R_st4;
	case 5:
		return R_st5;
	case 6:
		return R_st6;
	case 7:
		return R_st7;
	default:
		return R_none;
	}
}

void InstructionHandler::insertAssignmentWithAssociativeCommutativeExpression(MathematicalOperator mathematicalOperator, InstructionVectorPtr& inst_list){
	tmpSubscript++;
	AssignmentPtr tmpAssignment = std::make_shared<Assignment>(getTmpVariable(), opHandler.ExpressionFromOperand(0));
	inst_list->push_back(tmpAssignment);
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(opHandler.ExpressionFromOperand(0));
	operands->push_back(opHandler.ExpressionFromOperand(1));

	if(operands->at(1)->type == NUMERIC_CONSTANT){
		if(mathematicalOperator == OR_OPERATOR || mathematicalOperator == AND_OPERATOR || mathematicalOperator == XOR_OPERATOR){
			NumericConstantPtr num2 = std::dynamic_pointer_cast<NumericConstant>(operands->at(1));
			num2->isSigned = false;
		}
	}

	ExpressionPtr rhsExpression = std::shared_ptr<Expression>();
	switch(mathematicalOperator){
	case ADDITION_OPERATOR:
		{
			ExpressionPtr op2 = operands->at(1);
			if(op2->type == NUMERIC_CONSTANT){
				ExpressionPtr op1 = operands->at(0);
				NumericConstantPtr num2 = std::dynamic_pointer_cast<NumericConstant>(op2);
				if(op1->size_in_bytes == 1){
					num2->value = (char)(num2->value);
				}else if(op1->size_in_bytes == 2){
					num2->value = (short)(num2->value);
				}
			}
			rhsExpression = std::make_shared<AdditionExpression>(operands);
		}
		break;
	case MULTIPLICATION_OPERATOR:
		{
			rhsExpression = std::make_shared<MultiplicationExpression>(operands);
		}
		break;
	case OR_OPERATOR:
		{
			rhsExpression = std::make_shared<ORExpression>(operands);
		}
		break;
	case XOR_OPERATOR:
		{
			operands->operator[](0)->expressionType = getSignedIntegerType();
			operands->operator[](1)->expressionType = getSignedIntegerType();
			rhsExpression = std::make_shared<XORExpression>(operands);
		}
		break;
	case AND_OPERATOR:
		{
			rhsExpression = std::make_shared<ANDExpression>(operands);
		}
		break;
	default:
		msg("unknown mathematical operator at %x.\n", cmd.ea);
	}
	AssignmentPtr result = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), rhsExpression);
	inst_list->push_back(result);
}

void InstructionHandler::insertAssignmentWithBinaryNonCommutativeExpression(MathematicalOperator mathematicalOperator, InstructionVectorPtr& inst_list){
	tmpSubscript++;
	AssignmentPtr tmpAssignment = std::make_shared<Assignment>(getTmpVariable(), opHandler.ExpressionFromOperand(0));
	inst_list->push_back(tmpAssignment);
	ExpressionPtr lhsOperand = opHandler.ExpressionFromOperand(0);
	switch(mathematicalOperator){
	case SUBTRACTION_OPERATOR:
		{
			AssignmentPtr result = std::make_shared<Assignment>(lhsOperand, getSubtractionExpression(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1)));
			inst_list->push_back(result);
		}
		break;
	/*case DIVISION_OPERATOR:
		inst_list->push_back(new Assignment(lhsOperand, getDivisionExpression()));
		break;
	case EXPONENTIATION_OPERATOR:
		{
			Expression rhsOperand = new BinaryNonCommutativeExpression("^", opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
			inst_list->push_back(new Assignment(lhsOperand, rhsOperand));
		}
		break;
	case REMAINDER_OPERATOR:
		{
			Expression rhsOperand = new BinaryNonCommutativeExpression("%", opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
			inst_list->push_back(new Assignment(lhsOperand, rhsOperand));
		}
		break;*/
	default:
		msg("unknown mathematical operator at %x.\n", cmd.ea);
	}
}

ExpressionPtr InstructionHandler::getSubtractionExpression(ExpressionPtr firstOperand, ExpressionPtr secondOperand){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(firstOperand);
	if(secondOperand->type == NUMERIC_CONSTANT){
		NumericConstantPtr secondOperandNumericConstant = std::dynamic_pointer_cast<NumericConstant>(secondOperand);
		secondOperandNumericConstant->value = -1 * secondOperandNumericConstant->value;
		operands->push_back(secondOperandNumericConstant);
	}
	else{
		NegationExpressionPtr negatedSecondOperand = std::make_shared<NegationExpression>(secondOperand);
		operands->push_back(negatedSecondOperand);
	}

	AdditionExpressionPtr result = std::make_shared<AdditionExpression>(operands);
	return result;
}

ExpressionPtr InstructionHandler::getDivisionExpression(ExpressionPtr divident, ExpressionPtr divisor){
	DivisionExpressionPtr divExp = std::make_shared<DivisionExpression>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
	divExp->firstOperand->expressionType = (cmd.itype == NN_div ? getUnsignedIntegerType() : getSignedIntegerType());
	divExp->secondOperand->expressionType = (cmd.itype == NN_div ? getUnsignedIntegerType() : getSignedIntegerType());
	return divExp;
}

void InstructionHandler::handle_ADD(InstructionVectorPtr& inst_list){
	if(!(cmd.Operands[1].type == o_imm && cmd.Operands[1].value == 0)){
		if(opHandler.operandsEqual(0, 1)){
			ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
			NumericConstantPtr two = std::make_shared<NumericConstant>(2);
			operands->push_back(two);
			operands->push_back(opHandler.ExpressionFromOperand(0));
			MultiplicationExpressionPtr multByTwo = std::make_shared<MultiplicationExpression>(operands);
			AssignmentPtr assignInst = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multByTwo);
			inst_list->push_back(assignInst);
			ExpressionPtr nullPointer = std::shared_ptr<Expression>();
			ADDFlagMacroPtr addFlagMacro = std::make_shared<ADDFlagMacro>(nullPointer, nullPointer, opHandler.ExpressionFromOperand(0));
			inst_list->push_back(addFlagMacro);
		}
		else{
			//insertBinaryInstruction("+", inst_list, true);
			insertAssignmentWithAssociativeCommutativeExpression(ADDITION_OPERATOR, inst_list);
			ADDFlagMacroPtr addFlagMacro = std::make_shared<ADDFlagMacro>(getTmpVariable(), opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
			inst_list->push_back(addFlagMacro);
		}
	}
}

void InstructionHandler::handle_AND(InstructionVectorPtr& inst_list){
	op_t op2 = cmd.Operands[1];
	if(op2.type == o_imm && op2.value == 0){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		AssignmentPtr assignment = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), zero);
		inst_list->push_back(assignment);
		//inst_list->push_back(new Assignment(opHandler.ExpressionFromOperand(0), new NumericLiteral(0)));
		ExpressionPtr nullPointer = std::shared_ptr<Expression>();
		LogicalFlagMacroPtr logicalFlagMacro = std::make_shared<LogicalFlagMacro>(nullPointer, nullPointer, zero->deepcopy());
		inst_list->push_back(logicalFlagMacro);
		//inst_list->push_back(new LogicalFlagMacro(NULL, NULL, new NumericLiteral(0)));
	}
	else{
		op_t op1 = cmd.Operands[0];
		if(op1.type == o_reg && op1.reg == R_sp){
			//TODO think about a smarter alternative.
			return;
		}
		else{
			insertAssignmentWithAssociativeCommutativeExpression(AND_OPERATOR, inst_list);
			//insertBinaryInstruction("&", inst_list, true);
			/*inst_list->push_back(new Assignment(new LocalVariable("tmp"), opHandler.ExpressionFromOperand(0)));
			inst_list->push_back(new Assignment(opHandler.ExpressionFromOperand(0),
												new BinaryExpression("&", opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1))));*/
			LogicalFlagMacroPtr logicalFlagMacro = std::make_shared<LogicalFlagMacro>(getTmpVariable(), opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
			inst_list->push_back(logicalFlagMacro);
		}
	}
}

void InstructionHandler::handle_CALL(InstructionVectorPtr& inst_list){
	op_t op1 = cmd.Operands[0];
	if(op1.addr == cmd.ea + cmd.size){
		decrementStackPointer(inst_list);
		saveExpressionOnStack(inst_list, opHandler.ExpressionFromOperand(0));
	}
	else{
		ExpressionVectorPtr arguments = std::make_shared<std::vector<ExpressionPtr>>();
		ExpressionVectorPtr returns = std::make_shared<std::vector<ExpressionPtr>>();
		bool func_type_found = false;
		func_type_data_t fi;
		op_t op1 = cmd.Operands[0];
		ea_t addr;

		if(op1.type == o_mem || op1.type == o_far || op1.type == o_near){
			TypeDecoder td;
			//to delete
			/*if(op1.addr == 0x401448){
				char reg_dtyp = opHandler.getAddressingRegisterDType();
				RegisterPtr regAX = std::make_shared<Register>(R_ax, reg_dtyp);
				returns->push_back(regAX);

				arguments->push_back(std::make_shared<Register>(R_ax, reg_dtyp));
				arguments->push_back(std::make_shared<Register>(R_bx, reg_dtyp));
				arguments->push_back(std::make_shared<Register>(R_si, reg_dtyp));

				ExpressionPtr functionPointer = opHandler.ExpressionFromOperand(0);

				CallPtr instCALL = std::make_shared<Call>(functionPointer, arguments, returns);
				inst_list->push_back(instCALL);
				return;
			}*/

			if(get_func_type(op1.addr, &fi)){
				func_type_found = true;
				addr = op1.addr;
			}
		}
		else {
			//TODO: check further
			ea_t code_ref_addr = get_first_fcref_from(cmd.ea);
			if(code_ref_addr != BADADDR){
				// if(get_func_type(code_ref_addr, &fi, declarationString)){
				if(get_func_type(code_ref_addr, &fi)){
					func_type_found = true;
					addr = code_ref_addr;	
				}
			}
		}

		TypeDecoder typeDecoder;
		InstructionPtr stackCleaningInstruction = std::shared_ptr<Instruction>();
		ExpressionPtr functionPointer = opHandler.ExpressionFromOperand(0);
		char reg_dtyp = opHandler.getAddressingRegisterDType();
		if(func_type_found){
			for(func_type_data_t::iterator it = fi.begin() ; it != fi.end() ; ++it){
				if(it->argloc.is_reg()){
					RegisterPtr reg_arg = std::make_shared<Register>(it->argloc.reg1(), reg_dtyp);
					// TODO fix typedecoder
					reg_arg->expressionType = typeDecoder.decodeType(it->type);
					arguments->push_back(reg_arg);
				}
				if(it->argloc.is_stkoff()){
					RegisterPtr regSP = std::make_shared<Register>(R_sp, reg_dtyp);
					PointerExpPtr stackExp;
					if(it->argloc.stkoff() == 0){
						stackExp = std::make_shared<PointerExp>(regSP);
						stackExp->expressionType = typeDecoder.decodeType(it->type);
					}
					else{
						ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
						operands->push_back(regSP);
						NumericConstantPtr offsetExp = std::make_shared<NumericConstant>(it->argloc.stkoff());
						operands->push_back(offsetExp);
						AdditionExpressionPtr stackAddrExp = std::make_shared<AdditionExpression>(operands);
						stackExp = std::make_shared<PointerExp>(stackAddrExp);
						stackExp->expressionType = typeDecoder.decodeType(it->type);
					}
					arguments->push_back(stackExp);
				}
			}
			tinfo_t return_type = fi.rettype;

			if(is_purging_cc(fi.cc)){
				// msg("adding cleaning instructions(cc = %x)\n", fi.cc);
				ExpressionVectorPtr addOperands = std::make_shared<std::vector<ExpressionPtr>>();
				RegisterPtr regSP = std::make_shared<Register>(R_sp, reg_dtyp);
				addOperands->push_back(regSP);
				NumericConstantPtr offsetExp = std::make_shared<NumericConstant>(fi.stkargs);
				addOperands->push_back(offsetExp);
				AdditionExpressionPtr spPlusOffset = std::make_shared<AdditionExpression>(addOperands);
				RegisterPtr targetRegSP = std::make_shared<Register>(R_sp, reg_dtyp);
				stackCleaningInstruction = std::make_shared<Assignment>(targetRegSP, spPlusOffset);
			}
		}
		RegisterPtr regAX = std::make_shared<Register>(R_ax, reg_dtyp);
		regAX->expressionType = typeDecoder.decodeType(fi.rettype);
		returns->push_back(regAX);
		CallPtr instCALL = std::make_shared<Call>(functionPointer, arguments, returns);
		inst_list->push_back(instCALL);
		if(!isInstructionPointerNull(stackCleaningInstruction))
			inst_list->push_back(stackCleaningInstruction);
	}
}

void InstructionHandler::handle_CMP(InstructionVectorPtr& inst_list){
	/*inst_list->push_back(new Assignment(new LocalVariable("result"), new BinaryExpression("-", opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1))));
	inst_list->push_back(new SUBFlagMacro(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), new LocalVariable("result")));*/
	int ad = cmd.ea;
	ExpressionPtr nullPointer = std::shared_ptr<Expression>();
	SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), nullPointer);
	inst_list->push_back(subFlagMacro);
}

void InstructionHandler::handle_CMPS(InstructionVectorPtr& inst_list){
	/*inst_list->push_back(new Assignment(new LocalVariable("result"), new BinaryExpression("-", opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1))));
	inst_list->push_back(new SUBFlagMacro(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), new LocalVariable("result")));*/

	InstructionPtr string_func = getStringFunction("memncmp");
	inst_list->push_back(string_func);

	ExpressionPtr nullPointer = std::shared_ptr<Expression>();
	CallPtr string_func_call = std::dynamic_pointer_cast<Call>(string_func);
	SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(nullPointer, nullPointer, string_func_call->returns->at(0));
	inst_list->push_back(subFlagMacro);

	ExpressionPtr incDecOperand = getIncDecExpression();
	char reg_type = opHandler.getAddressingRegisterDType();
	
	RegisterPtr regDI = std::make_shared<Register>(R_di, reg_type);
	AssignmentPtr instDI = std::make_shared<Assignment>(regDI, getAddition(regDI->deepcopy(), incDecOperand));
	inst_list->push_back(instDI);
	
	RegisterPtr regSI = std::make_shared<Register>(R_si, reg_type);
	AssignmentPtr instSI = std::make_shared<Assignment>(regSI, getAddition(regSI->deepcopy(), incDecOperand->deepcopy()));
	inst_list->push_back(instSI);
	
	if(isREP_Prefix()){
		RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_type);
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		AssignmentPtr instNullifyCX = std::make_shared<Assignment>(regCX, zero);
		inst_list->push_back(instNullifyCX);
	}
}

void InstructionHandler::handle_ConditionalJump(ExpressionPtr lowLevelCondition, InstructionVectorPtr& inst_list){
	int targetNodeId = getNodeID(cmd.Operands[0].addr);
	ConditionalJumpPtr condJump = std::make_shared<ConditionalJump>(lowLevelCondition, targetNodeId);
	inst_list->push_back(condJump);
}

void InstructionHandler::handle_DEC(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(opHandler.ExpressionFromOperand(0));
	NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
	operands->push_back(minusOne);
	//operands->push_back(new NumericLiteral(-1));
	AdditionExpressionPtr addExp = std::make_shared<AdditionExpression>(operands);
	AssignmentPtr instDEC = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), addExp);
	inst_list->push_back(instDEC);
	/*inst_list->push_back(new Assignment(opHandler.ExpressionFromOperand(0),
										new AssociativeCommutativeExpression(operands, "+")));*/
	ExpressionPtr nullPointer = std::shared_ptr<Expression>();
	SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(nullPointer, nullPointer, opHandler.ExpressionFromOperand(0), false);
	inst_list->push_back(subFlagMacro);
}

void InstructionHandler::handle_DIV(InstructionVectorPtr& inst_list){
	char reg_dtyp = opHandler.getAddressingRegisterDType();
	RegisterPtr regDX = std::make_shared<Register>(R_dx, reg_dtyp);
	regDX->expressionType = (cmd.itype == NN_div ? getUnsignedIntegerType() : getSignedIntegerType());
	RemainderExpressionPtr remainderExp = std::make_shared<RemainderExpression>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
	remainderExp->firstOperand->expressionType = (cmd.itype == NN_div ? getUnsignedIntegerType() : getSignedIntegerType());
	remainderExp->secondOperand->expressionType = (cmd.itype == NN_div ? getUnsignedIntegerType() : getSignedIntegerType());
	AssignmentPtr instRemainder = std::make_shared<Assignment>(regDX, remainderExp);
	inst_list->push_back(instRemainder);

	ExpressionPtr divResult = opHandler.ExpressionFromOperand(0);
	divResult->expressionType = (cmd.itype == NN_div ? getUnsignedIntegerType() : getSignedIntegerType());
	AssignmentPtr instDivision = std::make_shared<Assignment>(divResult,
										getDivisionExpression(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1)));
	inst_list->push_back(instDivision);
}

void InstructionHandler::insertFPUStackAssignments(InstructionVectorPtr& inst_list, bool isPush){
	RegisterPtr prevReg, currentReg;
	if(isPush){
		for(int fpreg_id = FPU_Registers_Num - 1 ; fpreg_id > 0 ; fpreg_id--){
			currentReg = std::make_shared<Register>(getFPRegByIndex(fpreg_id));
			prevReg = std::make_shared<Register>(getFPRegByIndex(fpreg_id - 1));
			AssignmentPtr instPUSH = std::make_shared<Assignment>(currentReg, prevReg);
			inst_list->push_back(instPUSH);
		}
	}
	else{
		for(int fpreg_id = 0 ; fpreg_id < FPU_Registers_Num - 1; fpreg_id++){
			prevReg = std::make_shared<Register>(getFPRegByIndex(fpreg_id));
			currentReg = std::make_shared<Register>(getFPRegByIndex(fpreg_id + 1));
			AssignmentPtr instPOP = std::make_shared<Assignment>(prevReg, currentReg);
			inst_list->push_back(instPOP);
		}
	}
}

void InstructionHandler::handle_F2XM1(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	MathFunctionPtr powerTwo = std::make_shared<MathFunction>(TWO_X, opHandler.ExpressionFromOperand(0));
	operands->push_back(powerTwo);
	NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
	operands->push_back(minusOne);
	AdditionExpressionPtr powerTwoMinusOne = std::make_shared<AdditionExpression>(operands);
	
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), powerTwoMinusOne));
	/*ExpressionPtr target = opHandler.ExpressionFromOperand(0);
	target->expressionType = getFloatingPointType();
	AssignmentPtr instResult = std::make_shared<Assignment>(target, powerTwoMinusOne);
	inst_list->push_back(instResult);*/
}

void InstructionHandler::handle_FABS(InstructionVectorPtr& inst_list){
	MathFunctionPtr absoluteValue = std::make_shared<MathFunction>(ABS, opHandler.ExpressionFromOperand(0));
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), absoluteValue));
	/*ExpressionPtr target = opHandler.ExpressionFromOperand(0);
	target->expressionType = getFloatingPointType();
	AssignmentPtr instABS = std::make_shared<Assignment>(target, absoluteValue);
	inst_list->push_back(instABS);*/
}

void InstructionHandler::handle_FADD(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(opHandler.ExpressionFromOperand(0));
	operands->push_back(opHandler.ExpressionFromOperand(1));
	AdditionExpressionPtr additionExp = std::make_shared<AdditionExpression>(operands);
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), additionExp));
	
	/*AssignmentPtr instAdd = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), additionExp);
	inst_list->push_back(instAdd);*/
}

void InstructionHandler::handle_FADDP(InstructionVectorPtr& inst_list){
	handle_FADD(inst_list);
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FCHS(InstructionVectorPtr& inst_list){
	NegationExpressionPtr negatedOperand = std::make_shared<NegationExpression>(opHandler.ExpressionFromOperand(0));
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), negatedOperand));
	/*AssignmentPtr instNegate = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), negatedOperand);
	inst_list->push_back(instNegate);*/
}

void InstructionHandler::handle_FCOM(InstructionVectorPtr& inst_list, bool setEFLAGS, int numPops){
	FCOMFlagMacroPtr fcomFlagMacro = std::make_shared<FCOMFlagMacro>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), setEFLAGS);
	inst_list->push_back(fcomFlagMacro);
	if(numPops != 0){
		for(int fpreg_id = 0 ; fpreg_id < FPU_Registers_Num - numPops; fpreg_id++){
			RegisterPtr targetReg = std::make_shared<Register>(getFPRegByIndex(fpreg_id));
			RegisterPtr srcReg = std::make_shared<Register>(getFPRegByIndex(fpreg_id + numPops));
			AssignmentPtr instPOP = std::make_shared<Assignment>(targetReg, srcReg);
			inst_list->push_back(instPOP);
		}
	}
}

void InstructionHandler::handle_FCOS(InstructionVectorPtr& inst_list){
	MathFunctionPtr cosine = std::make_shared<MathFunction>(COS, opHandler.ExpressionFromOperand(0));
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), cosine));
	/*AssignmentPtr instCOS = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), cosine);
	inst_list->push_back(instCOS);*/
}

void InstructionHandler::handle_FDIV(InstructionVectorPtr& inst_list, bool isReverse){
	AssignmentPtr instDIV;
	if(isReverse){
		instDIV = getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0),
											   getDivisionExpression(opHandler.ExpressionFromOperand(1),
													  			     opHandler.ExpressionFromOperand(0)));
		/*instDIV = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0),
											   getDivisionExpression(opHandler.ExpressionFromOperand(1),
													  			     opHandler.ExpressionFromOperand(0)));*/
	}
	else{
		instDIV = getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0),
											   getDivisionExpression(opHandler.ExpressionFromOperand(0),
																     opHandler.ExpressionFromOperand(1)));
		/*instDIV = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0),
											   getDivisionExpression(opHandler.ExpressionFromOperand(0),
																     opHandler.ExpressionFromOperand(1)));*/
	}
	inst_list->push_back(instDIV);
}

void InstructionHandler::handle_FDIVP(InstructionVectorPtr& inst_list, bool isReverse){
	handle_FDIV(inst_list, isReverse);
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FISTTP(InstructionVectorPtr& inst_list){
	MathFunctionPtr truncateFn = std::make_shared<MathFunction>(TRUNCATE, opHandler.ExpressionFromOperand(0));
	inst_list->push_back(getAssignment_integer(opHandler.ExpressionFromOperand(0), truncateFn));
	/*AssignmentPtr instTruncate = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), truncateFn);
	inst_list->push_back(instTruncate);*/
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FLD(InstructionVectorPtr& inst_list){
	op_t op2 = cmd.Operands[1];
	if(op2.type == o_fpreg){
		if(opHandler.operandsEqual(0, 1)){
			insertFPUStackAssignments(inst_list, true);
		}
		else{
			tmpSubscript++;
			AssignmentPtr assignFtmp = std::make_shared<Assignment>(getFtmpVariable(), opHandler.ExpressionFromOperand(1));
			inst_list->push_back(assignFtmp);
			insertFPUStackAssignments(inst_list, true);
			AssignmentPtr restoreFtmp = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), getFtmpVariable());
			inst_list->push_back(restoreFtmp);
		}
	}
	else{
		insertFPUStackAssignments(inst_list, true);
		inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1)));
		/*AssignmentPtr instFLD = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
		inst_list->push_back(instFLD);*/
	}
}

void InstructionHandler::handle_FLDConstant(ExpressionPtr floatingPointConstant, InstructionVectorPtr& inst_list){
	insertFPUStackAssignments(inst_list, true);
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), floatingPointConstant));
	/*AssignmentPtr instFLD = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), floatingPointConstant);
	inst_list->push_back(instFLD);*/
}

void InstructionHandler::handle_FMUL(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(opHandler.ExpressionFromOperand(0));
	operands->push_back(opHandler.ExpressionFromOperand(1));
	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);

	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), multExp));
	/*AssignmentPtr instMUL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multExp);
	inst_list->push_back(instMUL);*/
}

void InstructionHandler::handle_FMULP(InstructionVectorPtr& inst_list){
	handle_FMUL(inst_list);
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FPATAN(InstructionVectorPtr& inst_list){
	RegisterPtr dividendReg = std::make_shared<Register>(getFPRegByIndex(1));
	RegisterPtr divisorReg = std::make_shared<Register>(getFPRegByIndex(0));
	MathFunctionPtr arctanOfDivision = std::make_shared<MathFunction>(ARCTAN, getDivisionExpression(dividendReg, divisorReg));

	RegisterPtr resultReg = std::make_shared<Register>(getFPRegByIndex(1));

	inst_list->push_back(getAssignment_floatingPoint(resultReg, arctanOfDivision));
	/*AssignmentPtr instATAN = std::make_shared<Assignment>(resultReg, arctanOfDivision);
	inst_list->push_back(instATAN);*/
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FPTAN(InstructionVectorPtr& inst_list){
	RegisterPtr srcReg = std::make_shared<Register>(getFPRegByIndex(0));
	MathFunctionPtr tanFn = std::make_shared<MathFunction>(TAN, srcReg);
	RegisterPtr dstReg = std::make_shared<Register>(getFPRegByIndex(0));
	
	inst_list->push_back(getAssignment_floatingPoint(dstReg, tanFn));
	/*AssignmentPtr instTAN = std::make_shared<Assignment>(dstReg, tanFn);
	inst_list->push_back(instTAN);*/
	
	NumericConstantPtr one = std::make_shared<NumericConstant>(1);
	handle_FLDConstant(one, inst_list);
}

void InstructionHandler::handle_FRNDINT(InstructionVectorPtr& inst_list){
	RegisterPtr srcReg = std::make_shared<Register>(getFPRegByIndex(0));
	MathFunctionPtr roundToInt = std::make_shared<MathFunction>(RNDINT, srcReg);
	RegisterPtr dstReg = std::make_shared<Register>(getFPRegByIndex(0));
	inst_list->push_back(getAssignment_integer(dstReg, roundToInt));
	/*AssignmentPtr instRoundToInt = std::make_shared<Assignment>(dstReg, roundToInt);
	inst_list->push_back(instRoundToInt);*/
}

void InstructionHandler::handle_FSIN(InstructionVectorPtr& inst_list){
	RegisterPtr srcReg = std::make_shared<Register>(getFPRegByIndex(0));
	MathFunctionPtr sinFn = std::make_shared<MathFunction>(SIN, srcReg);
	RegisterPtr dstReg = std::make_shared<Register>(getFPRegByIndex(0));
	
	inst_list->push_back(getAssignment_floatingPoint(dstReg, sinFn));
	/*AssignmentPtr instSIN = std::make_shared<Assignment>(dstReg, sinFn);
	inst_list->push_back(instSIN);*/
}

void InstructionHandler::handle_FSINCOS(InstructionVectorPtr& inst_list){
	tmpSubscript++;
	RegisterPtr srcReg = std::make_shared<Register>(getFPRegByIndex(0));
	MathFunctionPtr cosFn = std::make_shared<MathFunction>(COS, srcReg);
	inst_list->push_back(getAssignment_floatingPoint(getTmpVariable(), cosFn));
	/*AssignmentPtr assignFtmp = std::make_shared<Assignment>(getTmpVariable(), cosFn);
	inst_list->push_back(assignFtmp);*/
	handle_FSIN(inst_list);
	insertFPUStackAssignments(inst_list, true);
	RegisterPtr dstReg = std::make_shared<Register>(getFPRegByIndex(0));
	inst_list->push_back(getAssignment_floatingPoint(dstReg, getTmpVariable()));
	/*AssignmentPtr restoreFtmp = std::make_shared<Assignment>(dstReg, getTmpVariable());
	inst_list->push_back(restoreFtmp);*/
}

void InstructionHandler::handle_FST(InstructionVectorPtr& inst_list){
	if(!opHandler.operandsEqual(0, 1)){
		inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0)));
		/*AssignmentPtr instSET = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
		inst_list->push_back(instSET);*/
	}
}

void InstructionHandler::handle_FSTSW(InstructionVectorPtr& inst_list){
	intToFlagMapPtr flagMap = std::make_shared<std::map<int, FlagPtr>>();
	FlagPtr flagC0 = std::make_shared<Flag>(FPU_FLAG, C0);
	FlagPtr flagC2 = std::make_shared<Flag>(FPU_FLAG, C2);
	FlagPtr flagC3 = std::make_shared<Flag>(FPU_FLAG, C3);
	flagMap->insert(std::pair<int, FlagPtr>(8, flagC0));
	flagMap->insert(std::pair<int, FlagPtr>(10, flagC2));
	flagMap->insert(std::pair<int, FlagPtr>(14, flagC3));
	FlagConcatenationPtr fpuFlags = std::make_shared<FlagConcatenation>(flagMap);
	AssignmentPtr instSTSW = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), fpuFlags);
	inst_list->push_back(instSTSW);
}

void InstructionHandler::handle_FSTP(InstructionVectorPtr& inst_list){
	handle_FST(inst_list);
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FSUB(InstructionVectorPtr& inst_list, bool isReverse){
	ExpressionPtr subtractionExpression;
	if(isReverse){
		subtractionExpression = getSubtractionExpression(opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
	}
	else{
		subtractionExpression = getSubtractionExpression(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
	}
	inst_list->push_back(getAssignment_floatingPoint(opHandler.ExpressionFromOperand(0), subtractionExpression));
	/*AssignmentPtr instSUB = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), subtractionExpression);
	inst_list->push_back(instSUB);*/
}

void InstructionHandler::handle_FSUBP(InstructionVectorPtr& inst_list, bool isReverse){
	handle_FSUB(inst_list, isReverse);
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FTST(InstructionVectorPtr& inst_list){
	NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
	FCOMFlagMacroPtr fcomFlagMacro = std::make_shared<FCOMFlagMacro>(opHandler.ExpressionFromOperand(0), zero, false);
	inst_list->push_back(fcomFlagMacro);
}

void InstructionHandler::handle_FXCH(InstructionVectorPtr& inst_list){
	if(!opHandler.operandsEqual(0, 1)){
		tmpSubscript++;
		AssignmentPtr saveTmp = std::make_shared<Assignment>(getTmpVariable(), opHandler.ExpressionFromOperand(1));
		inst_list->push_back(saveTmp);
		AssignmentPtr assignment = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
		inst_list->push_back(assignment);
		AssignmentPtr restoreTMP = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), getTmpVariable());
		inst_list->push_back(restoreTMP);
	}
}

void InstructionHandler::handle_FYL2X(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	
	RegisterPtr multOp1 = std::make_shared<Register>(getFPRegByIndex(1));
	operands->push_back(multOp1);
	
	RegisterPtr regSt0 = std::make_shared<Register>(getFPRegByIndex(0));
	MathFunctionPtr multOp2 = std::make_shared<MathFunction>(LOG_2, regSt0);
	operands->push_back(multOp2);
	
	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);
	RegisterPtr dstReg = std::make_shared<Register>(getFPRegByIndex(1));
	inst_list->push_back(getAssignment_floatingPoint(dstReg, multExp));
	/*AssignmentPtr instFYL2X = std::make_shared<Assignment>(dstReg, multExp);
	inst_list->push_back(instFYL2X);*/
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_FYL2XP1(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr innerOperands = std::make_shared<std::vector<ExpressionPtr>>();
	RegisterPtr regSt0 = std::make_shared<Register>(getFPRegByIndex(0));
	innerOperands->push_back(regSt0);
	NumericConstantPtr one = std::make_shared<NumericConstant>(1);
	innerOperands->push_back(one);
	AdditionExpressionPtr addInsideLog = std::make_shared<AdditionExpression>(innerOperands);
	MathFunctionPtr logFn = std::make_shared<MathFunction>(LOG_2, addInsideLog);

	ExpressionVectorPtr outerOperands = std::make_shared<std::vector<ExpressionPtr>>();
	RegisterPtr multOp1 = std::make_shared<Register>(getFPRegByIndex(1));
	outerOperands->push_back(multOp1);
	outerOperands->push_back(logFn);
	MultiplicationExpressionPtr srcMultExp = std::make_shared<MultiplicationExpression>(outerOperands);
	RegisterPtr dstReg = std::make_shared<Register>(getFPRegByIndex(1));
	inst_list->push_back(getAssignment_floatingPoint(dstReg, srcMultExp));
	/*AssignmentPtr instFYL2XP1 = std::make_shared<Assignment>(dstReg, srcMultExp);
	inst_list->push_back(instFYL2XP1);*/
	insertFPUStackAssignments(inst_list, false);
}

void InstructionHandler::handle_INC(InstructionVectorPtr& inst_list){
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(opHandler.ExpressionFromOperand(0));
	NumericConstantPtr one = std::make_shared<NumericConstant>(1);
	operands->push_back(one);
	AdditionExpressionPtr incrementedOp = std::make_shared<AdditionExpression>(operands);
	AssignmentPtr instINC = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), incrementedOp);
	inst_list->push_back(instINC);

	ExpressionPtr nullPointer = std::shared_ptr<Expression>();
	ADDFlagMacroPtr addFlagMacro = std::make_shared<ADDFlagMacro>(nullPointer, nullPointer, opHandler.ExpressionFromOperand(0));
	inst_list->push_back(addFlagMacro);
}

void InstructionHandler::handle_JCXZ(char reg_dtyp, InstructionVectorPtr& inst_list){
	int targetNodeId = getNodeID(cmd.Operands[0].addr);
	NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
	RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_dtyp);
	HighLevelConditionPtr hlCondition = std::make_shared<HighLevelCondition>("==", regCX, zero);
	ConditionalJumpPtr condJump = std::make_shared<ConditionalJump>(hlCondition, targetNodeId);
	inst_list->push_back(condJump);
}

void InstructionHandler::handle_JMP(InstructionVectorPtr& inst_list){
	JumpPtr instJMP;
	if(cmd.Operands[0].type == o_near || cmd.Operands[0].type == o_far)
		instJMP = std::make_shared<DirectJump>(getNodeID(cmd.Operands[0].addr));
	else
		instJMP = std::make_shared<IndirectJump>(opHandler.ExpressionFromOperand(0), INDIRECT_JUMP);
	inst_list->push_back(instJMP);
}

void InstructionHandler::handle_LOOP(InstructionVectorPtr& inst_list){
	int targetNodeId = getNodeID(cmd.Operands[0].addr);
	char reg_type = opHandler.getAddressingRegisterDType();
	NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
	RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_type);
	HighLevelConditionPtr hlCondition = std::make_shared<HighLevelCondition>("!=", regCX, zero);
	ConditionalJumpPtr condJump = std::make_shared<ConditionalJump>(hlCondition, targetNodeId);
	inst_list->push_back(condJump);
}

void InstructionHandler::handle_MOV(InstructionVectorPtr& inst_list){
	if(!opHandler.operandsEqual(0, 1)){
		AssignmentPtr instMOV = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
		inst_list->push_back(instMOV);
	}
}

void InstructionHandler::handle_MOVS(InstructionVectorPtr& inst_list){
	inst_list->push_back(getStringFunction("memcpy"));
	ExpressionPtr incDecOperand = getIncDecExpression();
	char reg_type = opHandler.getAddressingRegisterDType();
	
	RegisterPtr regDI = std::make_shared<Register>(R_di, reg_type);
	AssignmentPtr instDI = std::make_shared<Assignment>(regDI, getAddition(regDI->deepcopy(), incDecOperand));
	inst_list->push_back(instDI);
	
	RegisterPtr regSI = std::make_shared<Register>(R_si, reg_type);
	AssignmentPtr instSI = std::make_shared<Assignment>(regSI, getAddition(regSI->deepcopy(), incDecOperand->deepcopy()));
	inst_list->push_back(instSI);
	
	if(isREP_Prefix()){
		RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_type);
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		AssignmentPtr instNullifyCX = std::make_shared<Assignment>(regCX, zero);
		inst_list->push_back(instNullifyCX);
	}
}

void InstructionHandler::handle_IMUL(InstructionVectorPtr& inst_list){
	//TODO EDX:EAX check further
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	if(cmd.Operands[2].type == o_void){
		operands->push_back(opHandler.ExpressionFromOperand(0));
		operands->push_back(opHandler.ExpressionFromOperand(1));
	}
	else{
		operands->push_back(opHandler.ExpressionFromOperand(1));
		operands->push_back(opHandler.ExpressionFromOperand(2));
	}

	for(unsigned int op_id = 0 ; op_id != operands->size() ; ++op_id){
		IntegralTypePtr intType = getSignedIntegerType();
		operands->operator[](op_id)->expressionType = intType;
	}

	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);
	AssignmentPtr instMUL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multExp);
	instMUL->lhsOperand->expressionType = getSignedIntegerType();
	inst_list->push_back(instMUL);
}

void InstructionHandler::handle_MUL(InstructionVectorPtr& inst_list){
	//TODO EDX:EAX check further
	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionPtr firstOperand = opHandler.ExpressionFromOperand(0);
	firstOperand->expressionType = getUnsignedIntegerType();
	operands->push_back(firstOperand);
	ExpressionPtr secondOperand = opHandler.ExpressionFromOperand(1);
	secondOperand->expressionType = getUnsignedIntegerType();
	operands->push_back(secondOperand);
	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);
	AssignmentPtr instMUL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multExp);
	instMUL->lhsOperand->expressionType = getUnsignedIntegerType();
	inst_list->push_back(instMUL);
}

void InstructionHandler::handle_NEG(InstructionVectorPtr& inst_list){
	NegationExpressionPtr negatedOp = std::make_shared<NegationExpression>(opHandler.ExpressionFromOperand(0));
	negatedOp->operand->expressionType = getSignedIntegerType();
	AssignmentPtr instNEG = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), negatedOp);
	instNEG->lhsOperand->expressionType = getSignedIntegerType();
	inst_list->push_back(instNEG);
	NEGFlagMacroPtr negFlagMacro = std::make_shared<NEGFlagMacro>(opHandler.ExpressionFromOperand(0));
	inst_list->push_back(negFlagMacro);
}

void InstructionHandler::handle_NOT(InstructionVectorPtr& inst_list){
	LogicalNotExpressionPtr notOp = std::make_shared<LogicalNotExpression>(opHandler.ExpressionFromOperand(0));
	notOp->operand->expressionType = getSignedIntegerType();
	AssignmentPtr instNOT = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), notOp);
	instNOT->lhsOperand->expressionType = getSignedIntegerType();
	inst_list->push_back(instNOT);
}

void InstructionHandler::handle_OR(InstructionVectorPtr& inst_list){
	op_t op2 = cmd.Operands[1];
	if(op2.type == o_imm && op2.value == 0xffffffff){
		NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
		AssignmentPtr instOR = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), minusOne);
		inst_list->push_back(instOR);
		ExpressionPtr nullPointer = std::shared_ptr<Expression>();
		LogicalFlagMacroPtr logicalFlagMacro = std::make_shared<LogicalFlagMacro>(nullPointer, nullPointer, minusOne->deepcopy());
		inst_list->push_back(logicalFlagMacro);
	}
	else{
		insertAssignmentWithAssociativeCommutativeExpression(OR_OPERATOR, inst_list);
		LogicalFlagMacroPtr logicalFlagMacro = std::make_shared<LogicalFlagMacro>(getTmpVariable(), opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
		inst_list->push_back(logicalFlagMacro);
	}
}

void InstructionHandler::handle_POP(InstructionVectorPtr& inst_list){
	int bytesPopped = opHandler.getOperandSize(0);
	char reg_dtyp = opHandler.getAddressingRegisterDType();

	RegisterPtr regSP = std::make_shared<Register>(R_sp, reg_dtyp);
	PointerExpPtr stackVar = std::make_shared<PointerExp>(regSP);
	AssignmentPtr instRestoreStkVar = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), stackVar);
	inst_list->push_back(instRestoreStkVar);

	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	operands->push_back(regSP->deepcopy());
	NumericConstantPtr numBytesPopped = std::make_shared<NumericConstant>(bytesPopped);
	operands->push_back(numBytesPopped);
	AdditionExpressionPtr newSP = std::make_shared<AdditionExpression>(operands);
	AssignmentPtr instIncSp = std::make_shared<Assignment>(regSP->deepcopy(), newSP);
	inst_list->push_back(instIncSp);
}

void InstructionHandler::handle_PUSH(InstructionVectorPtr& inst_list){
	decrementStackPointer(inst_list);
	saveExpressionOnStack(inst_list, opHandler.ExpressionFromOperand(0));
}

void InstructionHandler::handle_RET(InstructionVectorPtr& inst_list){
	RegisterPtr regAX = std::make_shared<Register>(R_ax, opHandler.getAddressingRegisterDType());
	ReturnPtr instRET = std::make_shared<Return>(regAX);
	inst_list->push_back(instRET);
}

void InstructionHandler::handle_ROTATE(InstructionVectorPtr& inst_list, std::string rotFunctionName){
	ExpressionVectorPtr arguments = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionVectorPtr returns = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionPtr rolFunctionPointer = std::make_shared<GlobalVariable>("rotFunctionName", -1);
	arguments->push_back(opHandler.ExpressionFromOperand(0));
	arguments->push_back(opHandler.ExpressionFromOperand(1));
	returns->push_back(opHandler.ExpressionFromOperand(0));
	inst_list->push_back(std::make_shared<Call>(rolFunctionPointer, arguments, returns));
}

void InstructionHandler::handle_SAR(InstructionVectorPtr& inst_list){
	ShiftExpressionPtr shiftExp = std::make_shared<ShiftExpression>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), true);
	shiftExp->firstOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	shiftExp->secondOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	AssignmentPtr instSAR = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), shiftExp);
	instSAR->lhsOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	inst_list->push_back(instSAR);

	/*ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionPtr firstOperand = opHandler.ExpressionFromOperand(0);
	firstOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	operands->push_back(firstOperand);

	ExpressionPtr secondOperand;
	if(cmd.Operands[1].type == o_imm){
		int exponent = -1 * cmd.Operands[1].value;
		secondOperand = std::make_shared<NumericConstant>(pow(2.0, exponent));
	}
	else{
		NumericConstantPtr two = std::make_shared<NumericConstant>(2);
		NegationExpressionPtr exponent = std::make_shared<NegationExpression>(opHandler.ExpressionFromOperand(1));
		exponent->operand->expressionType = getUnsignedIntegerType();
		secondOperand = std::make_shared<ExponentiationExpression>(two, exponent);
	}
	operands->push_back(secondOperand);
	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);
	AssignmentPtr instSAR = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multExp);
	instSAR->lhsOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	inst_list->push_back(instSAR);*/
}

void InstructionHandler::handle_SETCond(ExpressionPtr lowLevelCondition, InstructionVectorPtr& inst_list){
	NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
	NumericConstantPtr one = std::make_shared<NumericConstant>(1);
	TernaryExpressionPtr ternaryExp = std::make_shared<TernaryExpression>(lowLevelCondition, one, zero);
	AssignmentPtr instSETCond = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), ternaryExp);
	inst_list->push_back(instSETCond);
}

void InstructionHandler::handle_SAHF(InstructionVectorPtr& inst_list){
	intToFlagMapPtr flagMap = std::make_shared<std::map<int, FlagPtr>>();
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
	flagMap->insert(std::pair<int, FlagPtr>(0, flagCF));
	FlagPtr flagPF = std::make_shared<Flag>(CPU_FLAG, PF);
	flagMap->insert(std::pair<int, FlagPtr>(2, flagPF));
	FlagPtr flagAF = std::make_shared<Flag>(CPU_FLAG, AF);
	flagMap->insert(std::pair<int, FlagPtr>(4, flagAF));
	FlagPtr flagZF = std::make_shared<Flag>(CPU_FLAG, ZF);
	flagMap->insert(std::pair<int, FlagPtr>(6, flagZF));
	FlagPtr flagSF = std::make_shared<Flag>(CPU_FLAG, SF);
	flagMap->insert(std::pair<int, FlagPtr>(7, flagSF));
	FlagConcatenationPtr flags = std::make_shared<FlagConcatenation>(flagMap);
	RegisterPtr regAH = std::make_shared<Register>(R_ah);
	AssignmentPtr instSAHF = std::make_shared<Assignment>(flags, regAH);
	inst_list->push_back(instSAHF);
}

void InstructionHandler::handle_SAL(InstructionVectorPtr& inst_list){
	ShiftExpressionPtr shiftExp = std::make_shared<ShiftExpression>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), false);
	shiftExp->firstOperand->expressionType = (cmd.itype == NN_sal ? getSignedIntegerType() : getUnsignedIntegerType());
	shiftExp->secondOperand->expressionType = (cmd.itype == NN_sal ? getSignedIntegerType() : getUnsignedIntegerType());
	AssignmentPtr instSAL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), shiftExp);
	instSAL->lhsOperand->expressionType = (cmd.itype == NN_sal ? getSignedIntegerType() : getUnsignedIntegerType());
	inst_list->push_back(instSAL);
	

	/*ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionPtr firstOperand = opHandler.ExpressionFromOperand(0);
	firstOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	operands->push_back(firstOperand);
	
	ExpressionPtr secondOperand;
	if(cmd.Operands[1].type == o_imm){
		int exponent = cmd.Operands[1].value;
		secondOperand = std::make_shared<NumericConstant>(pow(2.0, exponent));
	}
	else{
		NumericConstantPtr two = std::make_shared<NumericConstant>(2);
		ExpressionPtr exponent = opHandler.ExpressionFromOperand(1);
		exponent->expressionType = getUnsignedIntegerType();
		secondOperand = std::make_shared<ExponentiationExpression>(two, exponent);
	}
	operands->push_back(secondOperand);
	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);
	AssignmentPtr instSAL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multExp);
	instSAL->lhsOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	inst_list->push_back(instSAL);*/
}

void InstructionHandler::handle_SBB(InstructionVectorPtr& inst_list){
	FlagVectorPtr testedFlags = std::make_shared<std::vector<FlagPtr>>();
	FlagPtr flagCF = std::make_shared<Flag>(CPU_FLAG, CF);
	testedFlags->push_back(flagCF);
	LowLevelConditionPtr lowLevelCondition = std::make_shared<LowLevelCondition>(testedFlags, Below);
	
	NumericConstantPtr minusOne = std::make_shared<NumericConstant>(-1);
	if(opHandler.operandsEqual(0, 1)){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		TernaryExpressionPtr teranyExp = std::make_shared<TernaryExpression>(lowLevelCondition, minusOne, zero);
		AssignmentPtr instSBB = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), teranyExp);
		inst_list->push_back(instSBB);
	}
	else{
		ExpressionVectorPtr tmpOperands = std::make_shared<std::vector<ExpressionPtr>>();
		tmpOperands->push_back(opHandler.ExpressionFromOperand(0));
		tmpOperands->push_back(minusOne);
		AdditionExpressionPtr firstOperandMinusOne = std::make_shared<AdditionExpression>(tmpOperands);
		tmpSubscript++;
		TernaryExpressionPtr ternaryExp = std::make_shared<TernaryExpression>(lowLevelCondition, firstOperandMinusOne, opHandler.ExpressionFromOperand(0));
		AssignmentPtr instModifyFirstOperand = std::make_shared<Assignment>(getTmpVariable(), ternaryExp);
		inst_list->push_back(instModifyFirstOperand);

		ExpressionVectorPtr sbbOperands = std::make_shared<std::vector<ExpressionPtr>>();
		sbbOperands->push_back(getTmpVariable());
		NegationExpressionPtr negatedSecondOperand = std::make_shared<NegationExpression>(opHandler.ExpressionFromOperand(1));
		sbbOperands->push_back(negatedSecondOperand);
		AdditionExpressionPtr sbbResult = std::make_shared<AdditionExpression>(sbbOperands);
		AssignmentPtr instSBB = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), sbbResult);
		inst_list->push_back(instSBB);
	}
	ExpressionPtr nullPointer = std::shared_ptr<Expression>();
	SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(nullPointer, nullPointer, opHandler.ExpressionFromOperand(0));
	inst_list->push_back(subFlagMacro);
}

void InstructionHandler::handle_SCAS(InstructionVectorPtr& inst_list){
	int operandSize = opHandler.getOperandSize(0);
	char reg_type = opHandler.getAddressingRegisterDType();
	ExpressionPtr bufferOperand = std::make_shared<Register>(R_di, reg_type);
	bufferOperand->expressionType = getPointerType();
	ExpressionPtr elementOperand = operandSize != 1 ? std::make_shared<Register>(R_ax, reg_type) : std::make_shared<Register>(R_al);
	if(isREP_Prefix() || isREPNE_Prefix()){
		ExpressionVectorPtr scasParameters = std::make_shared<std::vector<ExpressionPtr>>();
		scasParameters->push_back(bufferOperand);
		scasParameters->push_back(elementOperand);

		ExpressionVectorPtr scasReturns = std::make_shared<std::vector<ExpressionPtr>>();
		LocalVariablePtr lengthVar = std::make_shared<LocalVariable>("len");
		scasReturns->push_back(lengthVar);
		LocalVariablePtr scasFunctionPointer = std::make_shared<LocalVariable>(isREPNE_Prefix() ? "len_first_match" : "len_first_mismatch");
		CallPtr instSCAS = std::make_shared<Call>(scasFunctionPointer, scasParameters, scasReturns);
		inst_list->push_back(instSCAS);

		ExpressionVectorPtr additionParameters = std::make_shared<std::vector<ExpressionPtr>>();
		RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_type);
		additionParameters->push_back(regCX);
		NegationExpressionPtr negatedLengthVar = std::make_shared<NegationExpression>(lengthVar->deepcopy());
		additionParameters->push_back(negatedLengthVar);
		AdditionExpressionPtr regCXAfterExecuting = std::make_shared<AdditionExpression>(additionParameters);
		AssignmentPtr instModifyCX = std::make_shared<Assignment>(regCX->deepcopy(), regCXAfterExecuting);
		inst_list->push_back(instModifyCX);
		//still needs to modify edi
	}
	else{
		PointerExpPtr bufferInMemory = std::make_shared<PointerExp>(bufferOperand);
		ExpressionPtr nullPointer = std::shared_ptr<Expression>();
		SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(bufferInMemory, elementOperand, nullPointer);
		inst_list->push_back(subFlagMacro);
	}
}

void InstructionHandler::handle_SHR(InstructionVectorPtr& inst_list){
	ShiftExpressionPtr shiftExp = std::make_shared<ShiftExpression>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), true);
	shiftExp->firstOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	shiftExp->secondOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	AssignmentPtr instSAL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), shiftExp);
	instSAL->lhsOperand->expressionType = (cmd.itype == NN_sar ? getSignedIntegerType() : getUnsignedIntegerType());
	inst_list->push_back(instSAL);

	/*ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionPtr firstOperand = opHandler.ExpressionFromOperand(0);
	firstOperand->expressionType = (cmd.itype == NN_shr ? getUnsignedIntegerType() : getSignedIntegerType());
	operands->push_back(firstOperand);
	
	ExpressionPtr secondOperand;
	if(cmd.Operands[1].type == o_imm){
		int exponent = -1 * cmd.Operands[1].value;
		secondOperand = std::make_shared<NumericConstant>(pow(2.0, exponent));
	}
	else{
		NumericConstantPtr two = std::make_shared<NumericConstant>(2);
		ExpressionPtr exponent = opHandler.ExpressionFromOperand(1);
		exponent->expressionType = getUnsignedIntegerType();
		NegationExpressionPtr negatedExponent = std::make_shared<NegationExpression>(exponent);
		secondOperand = std::make_shared<ExponentiationExpression>(two, negatedExponent);
	}
	operands->push_back(secondOperand);
	MultiplicationExpressionPtr multExp = std::make_shared<MultiplicationExpression>(operands);
	AssignmentPtr instSAL = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), multExp);
	instSAL->lhsOperand->expressionType = (cmd.itype == NN_shr ? getUnsignedIntegerType() : getSignedIntegerType());
	inst_list->push_back(instSAL);
	//insertAssignmentWithBinaryNonCommutativeExpression(SHIFT_RIGHT_OPERATOR, inst_list);
	SHRFlagMacroPtr shrFlagMacro = std::make_shared<SHRFlagMacro>(getTmpVariable(), opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
	inst_list->push_back(shrFlagMacro);*/
}

void InstructionHandler::handle_STOS(InstructionVectorPtr& inst_list){	
	inst_list->push_back(getStringFunction("memset"));
	char reg_type = opHandler.getAddressingRegisterDType();
	RegisterPtr regDI = std::make_shared<Register>(R_di, reg_type);
	AssignmentPtr instModifyDI = std::make_shared<Assignment>(regDI->deepcopy(), getAddition(regDI, getIncDecExpression()));
	inst_list->push_back(instModifyDI);
	if(isREP_Prefix()){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_type);
		AssignmentPtr instModifyCX = std::make_shared<Assignment>(regCX, zero);
		inst_list->push_back(instModifyCX);
	}
}

void InstructionHandler::handle_SUB(InstructionVectorPtr& inst_list){
	if(opHandler.operandsEqual(0, 1)){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		AssignmentPtr instNullifyOperand = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), zero);
		inst_list->push_back(instNullifyOperand);
		ExpressionPtr nullPointer = std::shared_ptr<Expression>();
		SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(nullPointer, nullPointer, zero->deepcopy());
		inst_list->push_back(subFlagMacro);
	}
	else{
		ExpressionPtr op1 = opHandler.ExpressionFromOperand(1);
		if(op1->type == NUMERIC_CONSTANT && std::dynamic_pointer_cast<NumericConstant>(op1)->value == 0){
			ExpressionPtr nullPointer = std::shared_ptr<Expression>();
			SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(nullPointer, nullPointer, opHandler.ExpressionFromOperand(0));
			inst_list->push_back(subFlagMacro);
		}
		else{
			insertAssignmentWithBinaryNonCommutativeExpression(SUBTRACTION_OPERATOR, inst_list);
			SUBFlagMacroPtr subFlagMacro = std::make_shared<SUBFlagMacro>(getTmpVariable(), opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
			inst_list->push_back(subFlagMacro);
		}
	}
}

void InstructionHandler::handle_TEST(InstructionVectorPtr& inst_list){
	ExpressionPtr nullPointer = std::shared_ptr<Expression>();
	op_t op2 = cmd.Operands[1];
	LogicalFlagMacroPtr logicalFlagMacro;
	if(op2.type == o_imm && op2.value == 0){
		// AND x, 0 --> x = 0
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		logicalFlagMacro = std::make_shared<LogicalFlagMacro>(nullPointer, nullPointer, zero);
	}
	else if(opHandler.operandsEqual(0, 1)){
		logicalFlagMacro = std::make_shared<LogicalFlagMacro>(nullPointer, nullPointer, opHandler.ExpressionFromOperand(0));
	}
	else{
		logicalFlagMacro = std::make_shared<LogicalFlagMacro>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1), nullPointer);
	}
	inst_list->push_back(logicalFlagMacro);
}

void InstructionHandler::handle_XCHG(InstructionVectorPtr& inst_list){
	tmpSubscript++;
	AssignmentPtr instSaveToTmp = std::make_shared<Assignment>(getTmpVariable(), opHandler.ExpressionFromOperand(0));
	inst_list->push_back(instSaveToTmp);
	AssignmentPtr instExchangeOperands = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), opHandler.ExpressionFromOperand(1));
	inst_list->push_back(instExchangeOperands);
	AssignmentPtr instRestoreFromTmp = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(1), getTmpVariable());
	inst_list->push_back(instRestoreFromTmp);
}

void InstructionHandler::handle_XOR(InstructionVectorPtr& inst_list){
	LogicalFlagMacroPtr logicalFlagMacro;
	if(opHandler.operandsEqual(0, 1)){
		NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
		AssignmentPtr instNullifyOperand = std::make_shared<Assignment>(opHandler.ExpressionFromOperand(0), zero);
		inst_list->push_back(instNullifyOperand);
		ExpressionPtr nullPointer = std::shared_ptr<Expression>();
		logicalFlagMacro = std::make_shared<LogicalFlagMacro>(nullPointer, nullPointer, zero->deepcopy());
	}
	else{
		insertAssignmentWithAssociativeCommutativeExpression(XOR_OPERATOR, inst_list);
		logicalFlagMacro = std::make_shared<LogicalFlagMacro>(getTmpVariable(), opHandler.ExpressionFromOperand(1), opHandler.ExpressionFromOperand(0));
	}
	inst_list->push_back(logicalFlagMacro);
}

bool InstructionHandler::isREP_Prefix(){
	return (cmd.auxpref & aux_rep) != 0;
}

bool InstructionHandler::isREPNE_Prefix(){
	return (cmd.auxpref & aux_repne) != 0;
}

InstructionPtr InstructionHandler::getStringFunction(std::string functionName){
	int operandSize = opHandler.getOperandSize(0);
	char reg_type = opHandler.getAddressingRegisterDType();
	LocalVariablePtr functionPointer = std::make_shared<LocalVariable>(functionName);
	
	ExpressionVectorPtr functionParameters = std::make_shared<std::vector<ExpressionPtr>>();
	ExpressionVectorPtr functionReturns = std::make_shared<std::vector<ExpressionPtr>>();
	
	RegisterPtr regDI = std::make_shared<Register>(R_di, reg_type);
	regDI->expressionType = getPointerType();
	functionParameters->push_back(regDI);
	RegisterPtr secondParameter = std::shared_ptr<Register>();
	if(functionName.compare("memset") == 0)
		secondParameter = std::make_shared<Register>(operandSize == 1 ? R_al : R_ax, reg_type);
	else if(functionName.compare("memcpy") == 0){
		secondParameter = std::make_shared<Register>(R_si, reg_type);
		secondParameter->expressionType = getPointerType();
	}else if(functionName.compare("memncmp") == 0){
		secondParameter = std::make_shared<Register>(R_si, reg_type);
		secondParameter->expressionType = getPointerType();
		LocalVariablePtr result = std::make_shared<LocalVariable>("strcmp_result");
		functionReturns->push_back(result);
	}
	functionParameters->push_back(secondParameter);
	functionParameters->push_back(getLengthForStringInstructions());
	CallPtr stringFunction = std::make_shared<Call>(functionPointer, functionParameters, functionReturns);
	return stringFunction;
}

ExpressionPtr InstructionHandler::getLengthForStringInstructions(){
	int operandSize = opHandler.getOperandSize(0);
	char reg_type = opHandler.getAddressingRegisterDType();
	ExpressionPtr length = std::shared_ptr<Expression>();
	if(isREP_Prefix()){
		if(operandSize != 1){
			ExpressionVectorPtr lengthParameters = std::make_shared<std::vector<ExpressionPtr>>();
			RegisterPtr regCX = std::make_shared<Register>(R_cx, reg_type);
			lengthParameters->push_back(regCX);
			NumericConstantPtr opSize = std::make_shared<NumericConstant>(operandSize);
			lengthParameters->push_back(opSize);
			length = std::make_shared<MultiplicationExpression>(lengthParameters);
		}
		else{
			length = std::make_shared<Register>(R_cx, reg_type);
		}
	}
	else{
		length = std::make_shared<NumericConstant>(operandSize);
	}
	return length;
}

ExpressionPtr InstructionHandler::getAddition(ExpressionPtr op1, ExpressionPtr op2){
	ExpressionVectorPtr additionParameters = std::make_shared<std::vector<ExpressionPtr>>();
	additionParameters->push_back(op1);
	additionParameters->push_back(op2);
	AdditionExpressionPtr addExp = std::make_shared<AdditionExpression>(additionParameters);
	return addExp;
}

ExpressionPtr InstructionHandler::getIncDecExpression(){
	ExpressionPtr length = getLengthForStringInstructions();
	return length;
	NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
	FlagPtr flagDF = std::make_shared<Flag>(CPU_FLAG, DF);
	HighLevelConditionPtr incDecCondition = std::make_shared<HighLevelCondition>("==", flagDF, zero);
	
	
	NegationExpressionPtr negatedLength = std::make_shared<NegationExpression>(length);
	
	TernaryExpressionPtr incDecExpression = std::make_shared<TernaryExpression>(incDecCondition, length->deepcopy(), negatedLength);
	return incDecExpression;
}

LocalVariablePtr InstructionHandler::getTmpVariable(){
	LocalVariablePtr tmpVariable = std::make_shared<LocalVariable>("tmp" + boost::lexical_cast<std::string>(tmpSubscript));
	return tmpVariable;
}

LocalVariablePtr InstructionHandler::getFtmpVariable(){
	LocalVariablePtr ftmpVariable = std::make_shared<LocalVariable>("tmp" + boost::lexical_cast<std::string>(tmpSubscript));
	return ftmpVariable;
}


AssignmentPtr InstructionHandler::getAssignment_floatingPoint(ExpressionPtr target, ExpressionPtr source){
	target->expressionType = getFloatingPointType();
	return std::make_shared<Assignment>(target, source);
}

AssignmentPtr InstructionHandler::getAssignment_integer(ExpressionPtr target, ExpressionPtr source){
	target->expressionType = getSignedIntegerType();
	return std::make_shared<Assignment>(target, source);
}

TypePtr InstructionHandler::getFloatingPointType(){
	return std::make_shared<IntegralType>(DOUBLE_TYPE);
	/*uchar opcode = get_byte(cmd.ea);
	switch(cmd.itype){
	case NN_fld:
		switch(opcode){
		case 0xd9:
			return std::make_shared<IntegralType>(FLOAT);
		case 0xdd:
			return std::make_shared<IntegralType>(DOUBLE);
		default:
			return std::make_shared<IntegralType>(LONG_DOUBLE);
		}
	default:
		return getDefaultFloatingPointType();
	}*/
}

IntegralTypePtr InstructionHandler::getUnsignedIntegerType(int operandNumber){
	return std::make_shared<IntegralType>(INT_TYPE);
}

IntegralTypePtr InstructionHandler::getSignedIntegerType(int operandNumber){
	IntegralTypePtr intType = getUnsignedIntegerType(operandNumber);
	intType->isSigned = true;
	return intType;
}

PointerTypePtr InstructionHandler::getPointerType(){
	return std::make_shared<PointerType>();
}

void InstructionHandler::decrementStackPointer(InstructionVectorPtr& inst_list){
	int bytesPushed = opHandler.getOperandSize(0);
	char reg_dtyp = opHandler.getAddressingRegisterDType();

	ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
	RegisterPtr regSP = std::make_shared<Register>(R_sp, reg_dtyp);
	operands->push_back(regSP);
	NumericConstantPtr wordOffset = std::make_shared<NumericConstant>(-1 * bytesPushed);
	operands->push_back(wordOffset);
	AdditionExpressionPtr newSP = std::make_shared<AdditionExpression>(operands);
	AssignmentPtr instDecSp = std::make_shared<Assignment>(regSP->deepcopy(), newSP);
	inst_list->push_back(instDecSp);
}

void InstructionHandler::saveExpressionOnStack(InstructionVectorPtr& inst_list, ExpressionPtr expressionToSave){
	char reg_dtyp = opHandler.getAddressingRegisterDType();
	RegisterPtr regSP = std::make_shared<Register>(R_sp, reg_dtyp);
	PointerExpPtr stackVar = std::make_shared<PointerExp>(regSP);
	AssignmentPtr instSaveOnStack = std::make_shared<Assignment>(stackVar, expressionToSave);
	inst_list->push_back(instSaveOnStack);
}
