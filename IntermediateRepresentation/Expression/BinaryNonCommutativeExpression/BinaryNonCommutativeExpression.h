/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "../Expression.h"
#include "../ExpressionTypes.h"


class BinaryNonCommutativeExpression :
	public Expression
{
public:
	BinaryNonCommutativeExpression(ExpressionType _expressionType, ExpressionPtr _firstOperand, ExpressionPtr _secondOperand);
	virtual ~BinaryNonCommutativeExpression(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	//Expression* deepcopy();
	
	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	
	void simplifyOperands();

	bool equals(ExpressionPtr toThisExpression);

	//std::string operation;
	ExpressionPtr firstOperand;
	ExpressionPtr secondOperand;

	virtual std::string getOperationString() = 0;
//private:
//	bool replaceOperandNoSimplification(std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression, Expression** operandPtr);
};

