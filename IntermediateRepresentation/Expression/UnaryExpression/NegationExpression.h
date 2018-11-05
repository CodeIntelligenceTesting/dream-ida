/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "UnaryExpression.h"

class NegationExpression :
	public UnaryExpression
{
public:
	NegationExpression(ExpressionPtr _operand);
	virtual ~NegationExpression(void);

	ExpressionPtr deepcopy();
	//ExpressionPtr simplifyExpression();
	std::string getOperationString();
	std::string to_json();

	TypePtr getExpresstionType(TypeMapPtr typeMap){return operand->getExpresstionType(typeMap);};
	void updateExpressionType(TypePtr type, TypeMapPtr typeMap){operand->updateExpressionType(type, typeMap);};
};

