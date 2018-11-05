/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "AssociativeCommutativeExpression.h"

class ExponentiationExpression;

class MultiplicationExpression :
	public AssociativeCommutativeExpression
{
public:
	MultiplicationExpression(ExpressionVectorPtr operands);
	virtual ~MultiplicationExpression(void);

	ExpressionPtr deepcopy();
	ExpressionPtr simplifyExpression();
	std::string getOperationString();
	double getTotalConstantValue();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
private:
	ExpressionPtr simplifyExponentiationExpressionAfterAddingOneToExponent(const ExponentiationExpressionPtr& exponentiationExpression);
	ExpressionPtr simplifyMultiplicationOfTwoExponentiationExpressions(const ExponentiationExpressionPtr& firstExponentiationExpression, const ExponentiationExpressionPtr& secondExponentiationExpression);
	ExpressionPtr combineConstants();
	void removeNegatedOperands();
	MergeType getMergeType(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression);
};

