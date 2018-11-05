/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "AssociativeCommutativeExpression.h"

class XORExpression :
	public AssociativeCommutativeExpression
{
public:
	XORExpression(ExpressionVectorPtr _operands);
	virtual ~XORExpression(void);

	ExpressionPtr deepcopy();
	ExpressionPtr simplifyExpression();
	std::string getOperationString();

	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
private:
	ExpressionPtr combineConstants();
	MergeType getMergeType(const ExpressionPtr& firstExpression, const ExpressionPtr& secondExpression);
};

