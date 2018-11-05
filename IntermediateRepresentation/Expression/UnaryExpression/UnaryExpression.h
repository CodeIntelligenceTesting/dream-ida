/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "../Expression.h"
#include "../ExpressionTypes.h"

//Class representing single-operand expressions.
class UnaryExpression :
	public Expression
{
public:
	UnaryExpression(ExpressionType _expressionType, ExpressionPtr _operand);
	virtual ~UnaryExpression(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);

	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	bool equals(ExpressionPtr toThisExpression);
	//bool equalsDiscardConstant(Expression* toThisExpression);

	ExpressionPtr operand;
	std::string operation;

	virtual std::string getOperationString() = 0;
};
