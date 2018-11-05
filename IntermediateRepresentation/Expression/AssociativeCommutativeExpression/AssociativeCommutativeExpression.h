/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <set>
#include <vector>
#include <utility>

#include "../Expression.h"
#include "../ExpressionTypes.h"


enum MergeType{
	NO_MERGE_POSSIBLE,
	TWO_NUMERIC_CONSTANTS,
	//TWO_SYMBOLIC_CONSTANTS,
	TERNARY_EXPRESSION_WITH_CONSTANT,
	TWO_EQUAL_EXPRESSIONS,
	ADDITION_EXPRESSION_WITH_OPERAND_IN_MULTIPLICATION_EXPRESSION,
	ADDITION_TWO_MULTIPLICATION_EXPRESSIONS,
	MULTIPLICATION_EXPRESSION_WITH_EXPONENTIATION_EXPRESSION,
	MULTIPLICATION_TWO_EXPONENTIATION_EXPRESSIONS,
	LOGICAL_EXPRESSION_WITH_ITS_NEGATION
};

enum SignCombination{
	BOTH_POSITIVE,
	BOTH_NEGATIVE,
	FIRST_POSITIVE_SECOND_NEGATIVE,
	FIRST_NEGATIVE_SECOND_POSITIVE
};


typedef std::pair<Expression*, double> SimplificationEntry;

class AssociativeCommutativeExpression :
	public Expression
{
public:
	AssociativeCommutativeExpression(ExpressionVectorPtr _operands, ExpressionType _expressionType);
	virtual ~AssociativeCommutativeExpression(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	
	void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	void simplifyOperands();	
	bool equals(const ExpressionPtr& toThisExpression);
	
	//MergeType getConstantsMergeType(Expression* firstExpression, Expression* secondExpression);
	
	bool equalsDiscardConstants(const AssociativeCommutativeExpressionPtr& firstExpression, const AssociativeCommutativeExpressionPtr& secondExpression);
	void removeConstantOperands();

	virtual double getTotalConstantValue(){return 0;};

	ExpressionVectorPtr operands;
	virtual std::string getOperationString() = 0;
	std::string to_json();

	bool doesHavePointerOperand(TypeMapPtr typeMap);
};

