/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "AssociativeCommutativeExpression.h"

class MultiplicationExpression;

typedef std::pair<MergeType, SignCombination> MergeInfo;

class AdditionExpression :
	public AssociativeCommutativeExpression
{
public:
	AdditionExpression(ExpressionVectorPtr _operands);
	virtual ~AdditionExpression(void);

	ExpressionPtr deepcopy();
	ExpressionPtr simplifyExpression();
	std::string getOperationString();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
private:
	ExpressionPtr simplifyAdditionOfEqualExpressions(const ExpressionPtr& expressionToBeMerged, SignCombination signCombination);
	ExpressionPtr simplifyAdditionOfOperandAndMultiplicationExpression(const MultiplicationExpressionPtr& multiplicationExpression, const ExpressionPtr& operand, SignCombination signCombination);
	ExpressionPtr simplifyAdditionOfTwoMultiplicationExpressions(const MultiplicationExpressionPtr& firstMultExpression, const MultiplicationExpressionPtr& secondMultExpression, SignCombination signCombination);
	ExpressionPtr combineConstants();
	MergeInfo getMergeInfo(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression);
	bool canMergeWithMultiplicationExpression(const MultiplicationExpressionPtr& multExp, const ExpressionPtr& operandExpression);
};

