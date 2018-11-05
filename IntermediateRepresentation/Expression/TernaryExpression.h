/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "Expression.h"
#include "ExpressionTypes.h"

class TernaryExpression :
	public Expression
{
public:
	TernaryExpression(ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, ExpressionPtr _thirdOperand);
	virtual ~TernaryExpression(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string to_json();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();
	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool equals(ExpressionPtr toThisExpression);
	ExpressionPtr simplifyExpression();

	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	ExpressionPtr thirdOperand;
};

