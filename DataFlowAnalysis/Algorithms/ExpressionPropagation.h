/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "DataFlowAlgorithm.h"
#include "../DataStructures/Variable.h"

#include <fstream> //To Be deleted

#define MAXIMUM_EXPRESSION_LENGTH 3

struct PropagationUnit{
	std::string definitionName;
	int definitionSubscript;
	AssignmentPtr definingAssignment;
	InstructionPtr usingInstruction;

	PropagationUnit(std::string _definitionName, int _definitionSubscript, AssignmentPtr _definingAssignment, InstructionPtr _usingInstruction){
		definitionName = _definitionName;
		definitionSubscript = _definitionSubscript;
		definingAssignment = _definingAssignment;
		usingInstruction = _usingInstruction;
	}
};

typedef std::shared_ptr<PropagationUnit> PropagationUnitPtr;

class ExpressionPropagation :
	public DataFlowAlgorithm
{
public:
	ExpressionPropagation(ControlFlowGraphPtr _controlFlowGraph,
						  definitionsMapPtr _definitionsMap,
						  usesMapPtr _usesMap
						  /*std::set<Variable>* _phiVariables*/);
	virtual ~ExpressionPropagation(void);

	void applyAlgorithm(){};
	void applyAlgorithmNoMemoryVariables();
	void applyAlgorithmWithMemoryVariables();

	bool includePointers;
private: 
	std::set<Variable> referencedVariables;
	//std::set<Variable>* phiVariables;
	bool doesDefiningInstructionContainMemoryVariable(const std::string& defName, int defSubscript);
	bool propagateDefinition(const std::string& defName, int defSubscript);
	void propagateDefinitionToUse(PropagationUnitPtr& propagationUnit);
	void updateUsingInstruction(PropagationUnitPtr& propagationUnit, std::vector<ExpressionPtr> &addedElements_out, std::vector<ExpressionPtr> &replacedElements_out);
	void addExpressionsToUsingInstruction(std::vector<ExpressionPtr> &expListToBeAdded, const InstructionPtr &parentInst);
	bool canPropagateDefinitionToUse(const PropagationUnitPtr& propagationUnit);

	//bool isVariableInAPointerAddressExpression(std::string defName, int defSubscript, Expression* expToBePropagated, Instruction* usingInstruction);
	bool doesPropagationResultInMultipePointers(const PropagationUnitPtr& propagationUnit/*std::string defName, int defSubscript, Expression* expToBePropagated, Instruction* usingInstruction*/);
	bool doesPropagatePhiResourceIntoConditionalThroughOverwritingStatement(const PropagationUnitPtr& propagationUnit);
	void getPhiArgumentChain(const Phi_FunctionPtr& phi, std::vector<ExpressionPtr>& expChain);
	bool doesPropagationResultExceedMaximumExpressionLength(const PropagationUnitPtr& propagationUnit);
	bool doesPropagationSimplifyInstruction(const PropagationUnitPtr& propagationUnit);
	//bool doesContainPhiResource(Expression* expression);
	bool isDefinitionUsedInPointerInUsingInstruction(const PropagationUnitPtr& propagationUnit);
	bool isDefinitionUsedInPointer(const PropagationUnitPtr& propagationUnit, const PointerExpPtr& pointerExpression);
	bool doesPropagatePhiResourceOverPhiFunction(const PropagationUnitPtr& propagationUnit);
	bool isAssignmentWithMultipleUsesAndMultipleExpressions(const AssignmentPtr& assignment);
	bool isAssignmentWithEqualSizedOperands(const AssignmentPtr& assignment);
	bool isAssignmentWithPointerOfLessThanDWord(const AssignmentPtr& assignment);

	bool isAssignment(const InstructionPtr& instruction);
	bool isAssignmentOfAddressExpression(const AssignmentPtr& assignment);
	bool isAssignmentWithConstantUpdate(const AssignmentPtr& assignment);
	bool hasAlias(const std::string& variableName, int variableSubscript);

	bool haveUsesOtherThanVariableOrConstant(const ExpressionPtr& var, std::vector<InstructionPtr> &inst_list);
	bool useOnlySameVariableOrConstant(const ExpressionPtr& var, std::vector<ExpressionPtr> &expr_list);

	bool isStackPointer(const ExpressionPtr& expr);
	bool doesIncludeStackPointer(std::vector<ExpressionPtr> &expr_list);
	int getNumberOfSimpleExpressions(std::vector<ExpressionPtr> &expr_list);

	//bool isFunctionUsedOnlyInCondition(const InstructionPtr &inst);
	void removeUndefinedExpressions(std::vector<ExpressionPtr> &fromThisVector);
	void computeSymetricDifference(std::vector<ExpressionPtr> &firstVector, std::vector<ExpressionPtr> &secondVector);
	std::vector<ExpressionPtr>::iterator getPosition(const ExpressionPtr &expression, std::vector<ExpressionPtr> &expressionVector);
	//void updateUsesOfReplacedExpression(Expression* replacedExpression);
	//void removeDeadReplacedExpressions(std::vector<Expression*>* replacedExpressions);
	void updateReplacedExpressionsUsesMap(std::vector<ExpressionPtr> &removedExpressions, const InstructionPtr &usingInstruction);

	void copyVector(std::vector<ExpressionPtr> &srcVector, std::vector<ExpressionPtr> &dstVector);

	void replaceFunctionCallPointer(PropagationUnitPtr &propagationUnit);
	void replaceFunctionCallParameters(PropagationUnitPtr &propagationUnit);

	void getReferencedVariables();
	void printVector(std::vector<Expression*>* vec);
	void printInstructionVector(std::vector<Instruction*>* vec);


	std::ofstream out;
};

