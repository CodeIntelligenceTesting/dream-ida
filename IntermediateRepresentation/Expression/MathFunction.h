/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <vector>

#include "Expression.h"

enum MathFunctionType{
	ABS,
	ARCTAN,
	COS,
	LN,
	LOG_2,
	LOG_10,
	PREM,
	RNDINT,
	SIN,
	SQRT,
	TAN,
	TRUNCATE,
	TWO_X
};

class MathFunction :
	public Expression
{
public:
	MathFunction(MathFunctionType _mathFunctionType, ExpressionPtr _argument);
	virtual ~MathFunction(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();

	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);


	MathFunctionType mathFunctionType;
	ExpressionPtr argument;

private:
	std::string getFunctionName();
};

