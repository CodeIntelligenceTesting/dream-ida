/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "UnaryExpression.h"

class AddressExpression :
	public UnaryExpression
{
public:
	AddressExpression(ExpressionPtr _operand);
	virtual ~AddressExpression(void);

	ExpressionPtr deepcopy();
	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){};
	std::string getOperationString();
	std::string to_json();
	TypePtr getExpresstionType(TypeMapPtr typeMap);
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
};

