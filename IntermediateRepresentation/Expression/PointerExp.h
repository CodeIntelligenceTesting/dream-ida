/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

#include "Expression.h"
#include "ExpressionTypes.h"

class PointerExp :
	public Expression
{
public:
	PointerExp(ExpressionPtr _addressExpression);
	virtual ~PointerExp(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string to_json();
	std::string getName();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();
	//void replaceChildExpression(std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression);
	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	//bool replaceChildExpressionNoSimplification(std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression);
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	ExpressionPtr simplifyExpression();
	//Expression* addressExpression;
	ExpressionPtr addressExpression;
	bool hasName;
	//int subscript;

	void updateExpressionType(TypePtr type, TypeMapPtr typeMap);
	void set_expression_size(int byte_size);
};

