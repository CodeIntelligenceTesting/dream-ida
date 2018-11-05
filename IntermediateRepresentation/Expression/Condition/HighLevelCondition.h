/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "Condition.h"

class HighLevelCondition :
	public Condition
{
public:
	HighLevelCondition(std::string _comparisonOperand, ExpressionPtr _firstOperand, ExpressionPtr _secondOperand, bool _isUnsigned = false);
	virtual ~HighLevelCondition(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();

	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();
	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	bool isConditionTrue();
	bool isConditionFalse();

	std::string to_json();

	std::string comparisonOperand;
	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;
	bool isUnsigned;
private:
	bool areOperandsConstants();
//	bool replaceOperandNoSimplification(std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression, Expression** operandPtr);
};

