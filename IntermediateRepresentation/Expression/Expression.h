/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include <string.h>
#include <vector>

#include <ida.hpp>
#include <idp.hpp>
#include <lines.hpp>

#include "ExpressionTypes.h"
#include "../IRSharedPointers.h"
#include "../Enumerations/CodeColors.h"
#include "../../Shared/ColoredStrings.h"
#include "../../DataFlowAnalysis/MathematicalSimplification/SimplificationFeasibilityChecks.h"
#include "../../DataFlowAnalysis/MathematicalSimplification/localMathematicalSimplifications.h"

#include "../../TypeAnalysis/TypeSharedPointers.h"
#include "../../TypeAnalysis/Types/TypeBase.h"
#include "../../TypeAnalysis/TypeMap.h"

#define NO_SUBSCRIPT -1
#define NO_BLOCK_ID -1

//Abstract base class for all expressions.
class Expression
{
public:
	Expression(ExpressionType _type, int _subscript);
	virtual ~Expression(void);

	virtual std::string getExpressionString() = 0;
	virtual char* getColoredExpressionString() = 0;//delete
	virtual std::string getExpressionCOLSTR() = 0;
	virtual std::string getName(){return "";};
	virtual std::string to_json(){return "{\"expression_type\":\"LOWLEVEL\"}";}
	virtual void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){};
	
	virtual ExpressionPtr deepcopy() = 0;

	virtual std::string expressionTypeToString();
	virtual void set_expression_size(int byte_size);

	bool doesExpressionUseAPointer();

	bool isSameVariable(const std::string& variableName, int variableSubscript);

	virtual void replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){};
	virtual bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){return false;};
	
	virtual ExpressionPtr simplifyExpression(){return std::shared_ptr<Expression>();};
	
	virtual bool equals(const ExpressionPtr& toThisExpression);
	bool isMemoryVariable();

	bool isSimpleExpression;
	ExpressionType type;
	int subscript;
	int phiSourceBlockId;

	virtual TypePtr getExpresstionType(TypeMapPtr typeMap){return typeMap->getType(getName(), subscript);};
	virtual void updateExpressionType(TypePtr type, TypeMapPtr typeMap){typeMap->setType(getName(), subscript, type);};
	TypePtr expressionType;
	int size_in_bytes;
protected:
	virtual void replaceBySimplifiedExpression(ExpressionPtr& expressionToSimplifyPtr);
	virtual bool replaceOperandNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression, ExpressionPtr& operandPtr);
};
