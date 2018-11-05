/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "AssociativeCommutativeExpression.h"

class ANDExpression :
	public AssociativeCommutativeExpression
{
public:
	ANDExpression(ExpressionVectorPtr _operands, bool _isConditionalExpression = false);
	virtual ~ANDExpression(void);

	ExpressionPtr deepcopy();
	ExpressionPtr simplifyExpression();
	std::string getOperationString();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
private:
	bool isConditionalExpression;
	ExpressionPtr combineConstants();
	MergeType getMergeType(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression);
};

