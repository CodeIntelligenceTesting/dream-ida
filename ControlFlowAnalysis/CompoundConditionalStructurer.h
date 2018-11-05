/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../IntermediateRepresentation/IRSharedPointers.h"

//#include "EmptyNodesEliminator.h"

class CompoundConditionalStructurer
{
public:
	CompoundConditionalStructurer(ControlFlowGraphPtr _controlFlowGraph);
	virtual ~CompoundConditionalStructurer(void);

	void structureCompoundConditionals();

private:
	ControlFlowGraphPtr controlFlowGraph;
	//EmptyNodesEliminator nodesEliminator;
	//std::map<std::string, std::string> negatedConditionalOperationsMap;

	intVectorPtr getPostOrder();

	bool isHeadOf_A_AND_B(const NodePtr &head);
	bool isHeadOf_NOT_A_OR_B(const NodePtr &head);
	bool isHeadOf_A_OR_B(const NodePtr &head);
	bool isHeadOf_NOT_A_AND_B(const NodePtr &head);

	void merge_A_AND_B(NodePtr &head);
	void merge_NOT_A_OR_B(NodePtr &head);
	void merge_A_OR_B(NodePtr &head);
	void merge_NOT_A_AND_B(NodePtr &head);

	/*ExpressionPtr getNegatedExpression(ExpressionPtr &booleanExpression);

	ANDExpressionPtr getCompundCondition_AND(ExpressionPtr &conditionA, ExpressionPtr &conditionB);
	ORExpressionPtr getCompundCondition_OR(ExpressionPtr &conditionA, ExpressionPtr &conditionB);*/
};

