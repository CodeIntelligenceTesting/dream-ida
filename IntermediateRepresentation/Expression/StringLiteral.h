/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>

#include "Expression.h"
#include "ExpressionTypes.h"

class StringLiteral :
	public Expression
{
public:
	StringLiteral(std::string _value);
	virtual ~StringLiteral(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	std::string to_json();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	ExpressionPtr deepcopy();

	TypePtr getExpresstionType(TypeMapPtr typeMap);

private:
	std::string value;
	std::string replaceEscapeCharacters(std::string str);
};

