/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Enumerations/BranchTypes.h"

#include "Expression/AssociativeCommutativeExpression/AssociativeCommutativeExpression.h"
#include "Expression/AssociativeCommutativeExpression/AdditionExpression.h"
#include "Expression/AssociativeCommutativeExpression/ANDExpression.h"
#include "Expression/AssociativeCommutativeExpression/MultiplicationExpression.h"
#include "Expression/AssociativeCommutativeExpression/ORExpression.h"
#include "Expression/AssociativeCommutativeExpression/XORExpression.h"

#include "Expression/BinaryNonCommutativeExpression/ExponentiationExpression.h"
#include "Expression/BinaryNonCommutativeExpression/RemainderExpression.h"
#include "Expression/BinaryNonCommutativeExpression/DivisionExpression.h"
#include "Expression/BinaryNonCommutativeExpression/ShiftExpression.h"

#include "Expression/Expression.h"
#include "Expression/Condition/LowLevelCondition.h"
#include "Expression/Condition/HighLevelCondition.h"
#include "Expression/Flag.h"
#include "Expression/FlagConcatenation.h"
#include "Expression/LocalVariable.h"
#include "Expression/Location/GlobalVariable.h"
#include "Expression/Location/StackVariable.h"
#include "Expression/MathFunction.h"
//#include "Expression/NumericLiteral.h"
#include "Expression/Constant/NumericConstant.h"
#include "Expression/Constant/SymbolicConstant.h"
#include "Expression/PointerExp.h"
#include "Expression/Register.h"
#include "Expression/StringLiteral.h"
#include "Expression/TernaryExpression.h"

//#include "Expression/UnaryExpression.h"
#include "Expression/UnaryExpression/AddressExpression.h"
#include "Expression/UnaryExpression/LogicalNotExpression.h"
#include "Expression/UnaryExpression/NegationExpression.h"


#include "Instruction/Instruction.h"
#include "Instruction/Call.h"
#include "Instruction/BinaryInstruction/Assignment.h"
#include "Instruction/BinaryInstruction/ConditionalJump.h"
#include "Instruction/FlagMacro/ADDFlagMacro.h"
#include "Instruction/FlagMacro/LogicalFlagMacro.h"
#include "Instruction/FlagMacro/SUBFlagMacro.h"
#include "Instruction/FlagMacro/NEGFlagMacro.h"
#include "Instruction/FlagMacro/SALFlagMacro.h"
#include "Instruction/FlagMacro/SARFlagMacro.h"
#include "Instruction/FlagMacro/SHRFlagMacro.h"
#include "Instruction/FlagMacro/FCOMFlagMacro.h"
#include "Instruction/UnaryInstruction/Jump/DirectJump.h"
#include "Instruction/UnaryInstruction/Jump/IndirectJump.h"
#include "Instruction/UnaryInstruction/Jump/IndirectTableJump.h"
#include "Instruction/UnaryInstruction/Return.h"
#include "Instruction/Phi_Function.h"


#include "Node/Node.h"
#include "Node/OneWayNode.h"
#include "Node/TwoWayNode.h"
#include "Node/N_WayNode.h"
#include "Node/ReturnNode.h"
#include "Node/ExitNode.h"

#include "ControlFlowGraph.h"
