/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "UnaryExpression.h"

class LogicalNotExpression :
	public UnaryExpression
{
public:
	LogicalNotExpression(ExpressionPtr _operand, bool _isConditionalExpression = false);
	virtual ~LogicalNotExpression(void);

	ExpressionPtr deepcopy();
	ExpressionPtr simplifyExpression();
	std::string getOperationString();
	std::string to_json();

	TypePtr getExpresstionType(TypeMapPtr typeMap){return operand->getExpresstionType(typeMap);};
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap){operand->updateExpressionType(type, typeMap);};
private:
	bool isConditionalExpression;
};

