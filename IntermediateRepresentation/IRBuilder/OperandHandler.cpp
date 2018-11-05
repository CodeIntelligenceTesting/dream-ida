////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <math.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>

#include "../IRHeaders.h"
#include "OperandHandler.h"

#include "../../TypeAnalysis/Types/SimpleType/PointerType.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralType.h"

OperandHandler::OperandHandler(void)
{
}

OperandHandler::~OperandHandler(void)
{
}

ExpressionPtr OperandHandler::ExpressionFromOperand(int opIndex){
	if(isStkvar(getFlags(cmd.ea), opIndex)){
		ExpressionPtr result = createStackVariable(opIndex);
		//msg("stack variable: %s\n", result->getExpressionString().c_str());
		if(cmd.Operands[opIndex].hasSIB){
			ExpressionPtr sibExpression = getSIBExpression(opIndex);
			//msg("related sin: %s\n", sibExpression->getExpressionString().c_str());
			if(isEspOrEbp(sibExpression)){
				if(cmd.itype == NN_lea){
					result = std::make_shared<AddressExpression>(result);
				}
			}
			else{
				AddressExpressionPtr resultAddress = std::make_shared<AddressExpression>(result);
				result = updateSIBExpression(sibExpression, resultAddress, ADDITION_OPERATOR);
				if(cmd.itype != NN_lea){
					PointerExpPtr resultPointer = std::make_shared<PointerExp>(result);
					result = resultPointer;
				}
			}
		}
		else if(cmd.itype == NN_lea){
			AddressExpressionPtr resultAddress = std::make_shared<AddressExpression>(result);
			result = resultAddress;
		}
		result->set_expression_size(getOperandSize(opIndex));
		return result;
	}

	ExpressionPtr result;
	switch(cmd.Operands[opIndex].type){
	case o_reg:
		result = handle_o_reg(opIndex);
		break;
	case o_imm:
		result = handle_o_imm(opIndex);
		break;
	case o_near:
		result = handle_o_near(opIndex);
		break;
	case o_far:
		result = handle_o_far(opIndex);
		break;
	case o_mem:
		result = handle_o_mem(opIndex);
		break;
	case o_phrase:
		result = handle_o_phrase(opIndex);
		break;
	case o_displ:
		result = handle_o_displ(opIndex);
		break;
	case o_fpreg:
		result = handle_o_fpreg(opIndex);
		break;
	default:
		warning("OperandHandler::getExpressionFromOperand(int opIndex), wrong operand type\n");
		return NULL;
	}
	result->set_expression_size(getOperandSize(opIndex));
	return result;
}

ExpressionPtr OperandHandler::createStackVariable(int opIndex){
	std::string name;
	func_t* pfn = get_func(cmd.ea);
	ea_t offset = calc_stkvar_struc_offset(pfn, cmd.ea, opIndex);
	if(offset != BADADDR){
		struc_t* struc = get_frame(pfn);
		bool topmostMember = true;
		member_t* member;
		while(struc != NULL){
			member = get_member(struc, offset);
			if(member != NULL){
				
				if(topmostMember){
					topmostMember = false;
				}
				else{
					name += ".";
				}
					
				qstring memberName;
				if(get_member_name2(&memberName, member->id)){
					name += std::string(memberName.c_str());	
				}
				else{
					name += "NO_NAME";
				}

				offset -= member->soff;
				struc = get_sptr(member);
				if(cmd.itype == NN_lea)
					break;
			}
			else{
				break;
			}
		}
	}
	if(name.empty()){
		name = "namelessStackVariable";
	}
	StackVariablePtr stkVar = std::make_shared<StackVariable>(name);
	return stkVar;
}

ExpressionPtr OperandHandler::handle_o_reg(int opIndex){
	op_t operand = cmd.Operands[opIndex];
	RegisterPtr regPtr = std::make_shared<Register>(operand.reg, operand.dtyp);
	return regPtr;
}

ExpressionPtr OperandHandler::handle_o_imm(int opIndex){
	/*uval_t*/int operandValue = cmd.Operands[opIndex].value;
	refinfo_t refinfo;
	if(get_refinfo(cmd.ea, opIndex, &refinfo)){
		//handle string and global variables
		flags_t flags = getFlags(operandValue);
		if(isASCII(flags)){
			opinfo_t operandInfo;
			if(get_opinfo(operandValue, opIndex, flags, &operandInfo) != NULL){
				std::string operandString = getString(operandValue, operandInfo.strtype);
				if(!operandString.empty()){
					StringLiteralPtr stringVar = std::make_shared<StringLiteral>(operandString);
					return stringVar;
				}
			}
		}
		//else{
		AddressExpressionPtr globalVarAddr = std::make_shared<AddressExpression>(createGlobalVariable(operandValue));
		globalVarAddr->expressionType = getPointerType();
		return globalVarAddr;
		//}
		//return new UnaryExpression("&", createGlobalVariable(cmd.Operands[opIndex].value));
	}
	else{
		NumericConstantPtr constVar = std::make_shared<NumericConstant>(operandValue);
		return constVar;
		//return new NumericLiteral(cmd.Operands[opIndex].value);
	}
}

ExpressionPtr OperandHandler::handle_o_near(int opIndex){
	return handle_o_mem(opIndex);
}

ExpressionPtr OperandHandler::handle_o_far(int opIndex){
	return handle_o_mem(opIndex);
}

ExpressionPtr OperandHandler::handle_o_mem(int opIndex){
	if(cmd.itype != NN_lea && !cmd.Operands[opIndex].hasSIB){
		int operandValue = cmd.Operands[opIndex].addr;
		refinfo_t refinfo;
		if(get_refinfo(cmd.ea, opIndex, &refinfo)){
			//handle string and global variables
			flags_t flags = getFlags(operandValue);
			if(isASCII(flags)){
				opinfo_t operandInfo;
				if(get_opinfo(operandValue, opIndex, flags, &operandInfo) != NULL){
					std::string operandString = getString(operandValue, operandInfo.strtype);
					msg("operandInfo.strtype = %d\n", operandInfo.strtype);
					if(!operandString.empty()){
						StringLiteralPtr stringVar = std::make_shared<StringLiteral>(operandString);
						return std::make_shared<PointerExp>(stringVar);
					}
				}
			}
		}
	}

	ExpressionPtr result = createGlobalVariable(cmd.Operands[opIndex].addr);

	if(cmd.itype == NN_lea){
		result = std::make_shared<AddressExpression>(result);
		result->expressionType = getPointerType();
		//result = new UnaryExpression("&", result);
	}

	if(cmd.Operands[opIndex].hasSIB){
		result = updateSIBExpression(getSIBExpression(opIndex), result, ADDITION_OPERATOR);
		//result = new BinaryExpression("+", result, getSIBExpression(opIndex));
		result->expressionType = getPointerType();
		result = std::make_shared<PointerExp>(result);
	}

	return result;
}

ExpressionPtr OperandHandler::handle_o_phrase(int opIndex){
	return handle_o_displ(opIndex);
}

ExpressionPtr OperandHandler::handle_o_displ(int opIndex){
	op_t op = cmd.Operands[opIndex];
	ExpressionPtr result;
	
	if(op.hasSIB){
		result = getSIBExpression(opIndex);
	}
	else{
		result = std::make_shared<Register>(op.phrase, getAddressingRegisterDType());
	}

	/*ea_t*/int disp = op.addr;
	//TODO think about negative displacements
	if(disp != 0){
		////result = updateSIBExpression(result, new NumericLiteral(disp), ADDITION_OPERATOR);
		//NumericConstantPtr dispConstant = std::make_shared<NumericConstant>(disp);
		//result = updateSIBExpression(result, dispConstant, ADDITION_OPERATOR);
		///*std::string operation = "+";
		//result = new BinaryExpression(operation, result, new NumericLiteral(disp));*/
		//int operandValue = cmd.Operands[opIndex].value;
		refinfo_t refinfo;
		if(get_refinfo(cmd.ea, opIndex, &refinfo)){
			//handle global variables
			
			AddressExpressionPtr globalVarAddr = std::make_shared<AddressExpression>(createGlobalVariable(disp));
			globalVarAddr->expressionType = getPointerType();
			result = updateSIBExpression(result, globalVarAddr, ADDITION_OPERATOR);
		}
		else{
			NumericConstantPtr constVar = std::make_shared<NumericConstant>(disp);
			result = updateSIBExpression(result, constVar, ADDITION_OPERATOR);
		}
	}

	if(cmd.itype != NN_lea){
		result->expressionType = getPointerType();
		result = std::make_shared<PointerExp>(result);
	}
	
	if(result->type == POINTER){
		ExpressionPtr resultPtr = (std::dynamic_pointer_cast<PointerExp>(result))->addressExpression;
		if(resultPtr->type == ADDITION_EXPRESSION){
			ExpressionVectorPtr operands = (std::dynamic_pointer_cast<AdditionExpression>(resultPtr))->operands;
			if(operands->size() == 2){
				ExpressionPtr op1 = operands->front();
				ExpressionPtr op2 = operands->back();
				if(op1->type == NUMERIC_CONSTANT){
					NumericConstantPtr opConst1 = std::dynamic_pointer_cast<NumericConstant>(op1);
					if(opConst1->value <= MIN_POINTER_VALUE)
						op2->expressionType = getPointerType();
				}
				else if(op2->type == NUMERIC_CONSTANT){
					NumericConstantPtr opConst2 = std::dynamic_pointer_cast<NumericConstant>(op2);
					if(opConst2->value <= MIN_POINTER_VALUE)
						op1->expressionType = getPointerType();
				}
			}
		}
	}//also handle [reg + reg1*c1 + K]

	return result;
}

ExpressionPtr OperandHandler::handle_o_fpreg(int opIndex){
	switch(cmd.Operands[opIndex].reg){
	case 0:
		{
			RegisterPtr reg_st0 = std::make_shared<Register>(R_st0);
			return reg_st0;
		}
	case 1:
		{
			RegisterPtr reg_st1 = std::make_shared<Register>(R_st1);
			return reg_st1;
		}
	case 2:
		{
			RegisterPtr reg_st2 = std::make_shared<Register>(R_st2);
			return reg_st2;
		}
	case 3:
		{
			RegisterPtr reg_st3 = std::make_shared<Register>(R_st3);
			return reg_st3;
		}
	case 4:
		{
			RegisterPtr reg_st4 = std::make_shared<Register>(R_st4);
			return reg_st4;
		}
	case 5:
		{
			RegisterPtr reg_st5 = std::make_shared<Register>(R_st5);
			return reg_st5;
		}
	case 6:
		{
			RegisterPtr reg_st6 = std::make_shared<Register>(R_st6);
			return reg_st6;
		}
	case 7:
		{
			RegisterPtr reg_st7 = std::make_shared<Register>(R_st7);
			return reg_st7;
		}
	default:
		return NULL;
	}
}

//ExpressionPtr OperandHandler::getExpressionFromImmediate(int opIndex, uval_t value){}

ExpressionPtr OperandHandler::getSIBExpression(int opIndex){
	op_t op = cmd.Operands[opIndex];
	
	int scale = sib_scale(op);
	regnum_t index = sib_index(op);
	int base = sib_base(op);
	
	char reg_dtyp = getAddressingRegisterDType();

	ExpressionPtr result;
	if(base != 5 && index != 4){
		if(base == index){
			ExpressionVectorPtr resultOperands = std::make_shared<std::vector<ExpressionPtr>>();
			RegisterPtr regIndex = std::make_shared<Register>(index, reg_dtyp);
			regIndex->expressionType = getIntegerType();
			resultOperands->push_back(regIndex);
			NumericConstantPtr scaleConstant = std::make_shared<NumericConstant>(pow(2.0, scale) + 1); resultOperands->push_back(scaleConstant);
			result = std::make_shared<MultiplicationExpression>(resultOperands);
		}
		else{
			if(scale != 0){
				ExpressionVectorPtr resultOperands = std::make_shared<std::vector<ExpressionPtr>>();
				ExpressionVectorPtr innerOperands = std::make_shared<std::vector<ExpressionPtr>>();
				
				RegisterPtr regIndex = std::make_shared<Register>(index, reg_dtyp);
				regIndex->expressionType = getIntegerType();
				innerOperands->push_back(regIndex);
				NumericConstantPtr scaleConstant = std::make_shared<NumericConstant>(pow(2.0, scale)); innerOperands->push_back(scaleConstant);
				MultiplicationExpressionPtr innerMult = std::make_shared<MultiplicationExpression>(innerOperands);

				RegisterPtr regBase = std::make_shared<Register>(base, reg_dtyp); resultOperands->push_back(regBase);
				resultOperands->push_back(innerMult);
				AdditionExpressionPtr outerAdd = std::make_shared<AdditionExpression>(resultOperands);
				result = outerAdd;
			}
			else{
				ExpressionVectorPtr resultOperands = std::make_shared<std::vector<ExpressionPtr>>();
				RegisterPtr regBase = std::make_shared<Register>(base, reg_dtyp);
				resultOperands->push_back(regBase);
				RegisterPtr regIndex = std::make_shared<Register>(index, reg_dtyp);
				resultOperands->push_back(regIndex);
				result = std::make_shared<AdditionExpression>(resultOperands);
			}
		}
	}
	else{
		result = std::make_shared<Register>(index, reg_dtyp);
		if(scale != 0){
			ExpressionVectorPtr resultOperands = std::make_shared<std::vector<ExpressionPtr>>();
			result->expressionType = getIntegerType();
			resultOperands->push_back(result);
			NumericConstantPtr scaleConstant = std::make_shared<NumericConstant>(pow(2.0, scale));
			resultOperands->push_back(scaleConstant);
			result = std::make_shared<MultiplicationExpression>(resultOperands);
		}
	}
	return result;
}

bool OperandHandler::isEspOrEbp(ExpressionPtr exp){
	if(exp->type == REGISTER){
		RegisterPtr reg = std::dynamic_pointer_cast<Register>(exp);
		if(reg->regNo == R_sp || reg->regNo == R_bp)
			return true;
	}
	return false;
}

char OperandHandler::getAddressingRegisterDType(){
	if(ad16()){
		return dt_word;
	}
	else if(ad32()){
		return dt_dword;
	}
	else{
		//TODO think about 64 addressing mode
		return -1;
	}
}

bool OperandHandler::operandsEqual(int op1Index, int op2Index){
	op_t op1 = cmd.Operands[op1Index];
	op_t op2 = cmd.Operands[op2Index];
	//TODO imporove this
	if(op1.type == o_fpreg && op2.type == o_fpreg){
		return op1.reg == op2.reg;
	}

	if(op1.type != op2.type || op1.dtyp != op2.dtyp){
		return false;
	}
	else{
		if(op1.type == o_reg || op1.type == o_fpreg){
			return op1.reg == op2.reg;
		}
		else if(op1.type == o_imm){
			return op1.value == op2.value;
		}
		else if(op1.type == o_mem || op1.type == o_near || op1.type == o_far){
			return op1.addr == op2.addr;
		}
		else if(op1.type == o_phrase){
			return op1.reg == op2.reg && op1.phrase == op2.phrase;
		}
		else if(op1.type == o_displ){
			return op1.reg == op2.reg && op1.phrase == op2.phrase && op1.addr == op2.addr && op1.flags == op2.flags;
		}
		else{
			return false;
		}
	}
}

int OperandHandler::getOperandSize(int opIndex){
	switch(cmd.Operands[opIndex].dtyp){
	case dt_byte:
		return 1;
	case dt_word:
		return 2;
	case dt_dword:
		return 4;
	default:
		return -1;
	}
}

ExpressionPtr OperandHandler::createGlobalVariable(ea_t addr){
	func_t* func = get_func(cmd.ea);
	//ea_t addr = cmd.Operands[opIndex].addr;

	ExpressionPtr result;
	qstring name;
	if(func_contains(func, addr)){
		if(get_ea_name(&name, addr)){
			result = std::make_shared<GlobalVariable>(std::string(name.c_str()), addr);
		}
		else{
			char buffer [50];
			qsnprintf(buffer, 50, "gvar_%x", addr);
			//std::sprintf(buffer, "gvar_%x", addr);
			result = std::make_shared<GlobalVariable>(std::string(buffer), addr);
			//result = std::make_shared<GlobalVariable>("gvar_" + boost::lexical_cast<std::string>(addr), addr);
		}
	}
	else{
		if(get_ea_name(&name, addr)){
			result = std::make_shared<GlobalVariable>(std::string(name.c_str()), addr);
		}
		else{
			result = std::make_shared<GlobalVariable>("gvar_" + boost::lexical_cast<std::string>(addr), addr);
		}
	}
	return result;
}

ExpressionPtr OperandHandler::updateSIBExpression(ExpressionPtr sibExpression, ExpressionPtr expToAdd, MathematicalOperator addingOperator){
	if(sibExpression->type == ADDITION_EXPRESSION && addingOperator == ADDITION_OPERATOR){
		AdditionExpressionPtr sibAdditionExpression = std::dynamic_pointer_cast<AdditionExpression>(sibExpression);
		sibAdditionExpression->operands->push_back(expToAdd);
		return sibAdditionExpression;
	}
	else if(addingOperator == ADDITION_OPERATOR){
		ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
		operands->push_back(sibExpression);
		operands->push_back(expToAdd);
		AdditionExpressionPtr addExp = std::make_shared<AdditionExpression>(operands);
		return addExp;
	}
	else{
		msg("OperandHandler::updateSIBExpression(): unhandled case.\n");
		ExpressionPtr nullExp = std::shared_ptr<Expression>();
		return nullExp;
	}
}

std::string OperandHandler::getString(ea_t address, int32 strtype){
	size_t len = get_max_ascii_length(address, strtype, 0);
	boost::shared_array<char> str(new char[len + 1]);
	get_ascii_contents2(address, len, strtype, str.get(), len + 1);
	return str.get();
}

PointerTypePtr OperandHandler::getPointerType(){
	return std::make_shared<PointerType>();
}

IntegralTypePtr OperandHandler::getIntegerType(){
	return std::make_shared<IntegralType>(INT_TYPE);
}
