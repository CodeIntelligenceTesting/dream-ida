/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <ua.hpp>
#include <frame.hpp>
#include <name.hpp>
#include <intel.hpp>
#include <struct.hpp>
#include <pro.h>

#include "MathematicalOperators.h"
#include "../IRSharedPointers.h"
#include "../../TypeAnalysis/TypeSharedPointers.h"

class Expression;

class OperandHandler
{
public:
	OperandHandler(void);
	virtual ~OperandHandler(void);

	ExpressionPtr ExpressionFromOperand(int opIndex);
	bool operandsEqual(int op1Index, int op2Index);
	char getAddressingRegisterDType();
	int getOperandSize(int opIndex);
private:
	ExpressionPtr handle_o_reg(int opIndex);
	ExpressionPtr handle_o_imm(int opIndex);
	ExpressionPtr handle_o_near(int opIndex);
	ExpressionPtr handle_o_mem(int opIndex);
	ExpressionPtr handle_o_far(int opIndex);
	ExpressionPtr handle_o_phrase(int opIndex);
	ExpressionPtr handle_o_displ(int opIndex);
	ExpressionPtr handle_o_fpreg(int opIndex);

	//ExpressionPtr getExpressionFromImmediate(int opIndex, uval_t value);
	ExpressionPtr getSIBExpression(int opIndex);
	bool isEspOrEbp(ExpressionPtr exp);
	ExpressionPtr createGlobalVariable(ea_t addr);
	ExpressionPtr createStackVariable(int opIndex);
	ExpressionPtr updateSIBExpression(ExpressionPtr sibExpression, ExpressionPtr expToAdd, MathematicalOperator addingOperator);

	std::string getString(ea_t address, int32 strtype);

	PointerTypePtr getPointerType();
	IntegralTypePtr getIntegerType();
};

