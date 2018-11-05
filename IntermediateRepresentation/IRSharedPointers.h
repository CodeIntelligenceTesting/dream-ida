/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>

////////////////////////////Expression shared pointers///////////////////////////////////
class Expression;
typedef std::shared_ptr<Expression> ExpressionPtr;
	class AssociativeCommutativeExpression;
	typedef std::shared_ptr<AssociativeCommutativeExpression> AssociativeCommutativeExpressionPtr;
		class AdditionExpression;
		typedef std::shared_ptr<AdditionExpression> AdditionExpressionPtr;
		class ANDExpression;
		typedef std::shared_ptr<ANDExpression> ANDExpressionPtr;
		class MultiplicationExpression;
		typedef std::shared_ptr<MultiplicationExpression> MultiplicationExpressionPtr;
		class ORExpression;
		typedef std::shared_ptr<ORExpression> ORExpressionPtr;
		class XORExpression;
		typedef std::shared_ptr<XORExpression> XORExpressionPtr;
	class BinaryNonCommutativeExpression;
	typedef std::shared_ptr<BinaryNonCommutativeExpression> BinaryNonCommutativeExpressionPtr;
		class ExponentiationExpression;
		typedef std::shared_ptr<ExponentiationExpression> ExponentiationExpressionPtr;
		class RemainderExpression;
		typedef std::shared_ptr<RemainderExpression> RemainderExpressionPtr;
		class DivisionExpression;
		typedef std::shared_ptr<DivisionExpression> DivisionExpressionPtr;
		class ShiftExpression;
		typedef std::shared_ptr<ShiftExpression> ShiftExpressionPtr;
	class Condition;
	typedef std::shared_ptr<Condition> ConditionPtr;
		class HighLevelCondition;
		typedef std::shared_ptr<HighLevelCondition> HighLevelConditionPtr;
		class LowLevelCondition;
		typedef std::shared_ptr<LowLevelCondition> LowLevelConditionPtr;
	class Constant;
	typedef std::shared_ptr<Constant> ConstantPtr;
		class NumericConstant;
		typedef std::shared_ptr<NumericConstant> NumericConstantPtr;
		class SymbolicConstant;
		typedef std::shared_ptr<SymbolicConstant> SymbolicConstantPtr;
	class Location;
	typedef std::shared_ptr<Location> LocationPtr;
		class GlobalVariable;
		typedef std::shared_ptr<GlobalVariable> GlobalVariablePtr;
		class StackVariable;
		typedef std::shared_ptr<StackVariable> StackVariablePtr;
	class UnaryExpression;
	typedef std::shared_ptr<UnaryExpression> UnaryExpressionPtr;
		class AddressExpression;
		typedef std::shared_ptr<AddressExpression> AddressExpressionPtr;
		class LogicalNotExpression;
		typedef std::shared_ptr<LogicalNotExpression> LogicalNotExpressionPtr;
		class NegationExpression;
		typedef std::shared_ptr<NegationExpression> NegationExpressionPtr;
	class Flag;
	typedef std::shared_ptr<Flag> FlagPtr;
	class FlagConcatenation;
	typedef std::shared_ptr<FlagConcatenation> FlagConcatenationPtr;
	class LocalVariable;
	typedef std::shared_ptr<LocalVariable> LocalVariablePtr;
	class MathFunction;
	typedef std::shared_ptr<MathFunction> MathFunctionPtr;
	class PointerExp;
	typedef std::shared_ptr<PointerExp> PointerExpPtr;
	class Register;
	typedef std::shared_ptr<Register> RegisterPtr;
	class StringLiteral;
	typedef std::shared_ptr<StringLiteral> StringLiteralPtr;
	class TernaryExpression;
	typedef std::shared_ptr<TernaryExpression> TernaryExpressionPtr;


////////////////////////////Instruction shared pointers///////////////////////////////////
class Instruction;
typedef std::shared_ptr<Instruction> InstructionPtr;
	class BinaryInstruction;
	typedef std::shared_ptr<BinaryInstruction> BinaryInstructionPtr;
		class Assignment;
		typedef std::shared_ptr<Assignment> AssignmentPtr;
		class ConditionalJump;
		typedef std::shared_ptr<ConditionalJump> ConditionalJumpPtr;
	class FlagMacro;
	typedef std::shared_ptr<FlagMacro> FlagMacroPtr;
		class ADDFlagMacro;
		typedef std::shared_ptr<ADDFlagMacro> ADDFlagMacroPtr;
		class FCOMFlagMacro;
		typedef std::shared_ptr<FCOMFlagMacro> FCOMFlagMacroPtr;
		class FloatingPoint_SUBFlagMacro;
		typedef std::shared_ptr<FloatingPoint_SUBFlagMacro> FloatingPoint_SUBFlagMacroPtr;
		class IMULFlagMacro;
		typedef std::shared_ptr<IMULFlagMacro> IMULFlagMacroPtr;
		class INCFlagMacro;
		typedef std::shared_ptr<INCFlagMacro> INCFlagMacroPtr;
		class LogicalFlagMacro;
		typedef std::shared_ptr<LogicalFlagMacro> LogicalFlagMacroPtr;
		class MULFlagMacro;
		typedef std::shared_ptr<MULFlagMacro> MULFlagMacroPtr;
		class NEGFlagMacro;
		typedef std::shared_ptr<NEGFlagMacro> NEGFlagMacroPtr;
		class ROLFlagMacro;
		typedef std::shared_ptr<ROLFlagMacro> ROLFlagMacroPtr;
		class RORFlagMacro;
		typedef std::shared_ptr<RORFlagMacro> RORFlagMacroPtr;
		class SALFlagMacro;
		typedef std::shared_ptr<SALFlagMacro> SALFlagMacroPtr;
		class SARFlagMacro;
		typedef std::shared_ptr<SARFlagMacro> SARFlagMacroPtr;
		class SHRFlagMacro;
		typedef std::shared_ptr<SHRFlagMacro> SHRFlagMacroPtr;
		class SUBFlagMacro;
		typedef std::shared_ptr<SUBFlagMacro> SUBFlagMacroPtr;
	class UnaryInstruction;
	typedef std::shared_ptr<UnaryInstruction> UnaryInstructionPtr;
		class Return;
		typedef std::shared_ptr<Return> ReturnPtr;
		class Jump;
		typedef std::shared_ptr<Jump> JumpPtr;
			class DirectJump;
			typedef std::shared_ptr<DirectJump> DirectJumpPtr;
			class IndirectJump;
			typedef std::shared_ptr<IndirectJump> IndirectJumpPtr;
				class IndirectTableJump;
				typedef std::shared_ptr<IndirectTableJump> IndirectTableJumpPtr;
	class Call;
	typedef std::shared_ptr<Call> CallPtr;
	class Phi_Function;
	typedef std::shared_ptr<Phi_Function> Phi_FunctionPtr;

class Node;
typedef std::shared_ptr<Node> NodePtr;
	class OneWayNode;
	typedef std::shared_ptr<OneWayNode> OneWayNodePtr;
		class FallThroughNode;
		typedef std::shared_ptr<FallThroughNode> FallThroughNodePtr;
		class JumpNode;
		typedef std::shared_ptr<JumpNode> JumpNodePtr;
	class TwoWayNode;
	typedef std::shared_ptr<TwoWayNode> TwoWayNodePtr;
	class N_WayNode;
	typedef std::shared_ptr<N_WayNode> N_WayNodePtr;
	class ExitNode;
	typedef std::shared_ptr<ExitNode> ExitNodePtr;
	class ReturnNode;
	typedef std::shared_ptr<ReturnNode> ReturnNodePtr;


class ControlFlowGraph;
typedef std::shared_ptr<ControlFlowGraph> ControlFlowGraphPtr;
class InstructionHandler;
typedef std::shared_ptr<InstructionHandler> InstructionHandlerPtr;
class SSATransformer;
typedef std::shared_ptr<SSATransformer> SSATransformerPtr;

typedef std::shared_ptr<std::vector<ExpressionPtr>> ExpressionVectorPtr;
typedef std::shared_ptr<std::vector<FlagPtr>> FlagVectorPtr;
typedef std::shared_ptr<std::set<FlagPtr>> FlagSetPtr;
typedef std::shared_ptr<std::map<int, FlagPtr>> intToFlagMapPtr;
typedef std::shared_ptr<std::map<int, int>> intToIntMapPtr;
typedef std::shared_ptr<std::vector<InstructionPtr>> InstructionVectorPtr;
typedef std::shared_ptr<std::vector<int>> intVectorPtr;
typedef std::shared_ptr<std::vector<std::string>> stringVectorPtr;
typedef std::shared_ptr<std::set<int>> intSetPtr;
typedef std::shared_ptr<std::map<int, std::set<int>>> goToMapPtr; 
typedef std::shared_ptr<std::map<int, std::set<int>>> nodeToCasesMapPtr;

class qflow_chart_t;
typedef std::shared_ptr<qflow_chart_t> flowChartPtr;
typedef std::shared_ptr<std::map<int, NodePtr>> intToNodeMapPtr;

typedef std::shared_ptr<std::map<std::string, std::map<int, InstructionPtr>>> definitionsMapPtr;
typedef std::shared_ptr<std::map<std::string, std::map<int, std::vector<InstructionPtr>>>> usesMapPtr;

//class DefinitionResolver;
//typedef std::shared_ptr<DefinitionResolver> DefinitionResolverPtr;
