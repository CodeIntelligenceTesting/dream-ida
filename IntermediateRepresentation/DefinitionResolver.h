/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "IRSharedPointers.h"

class DefinitionResolver
{
public:
	DefinitionResolver(definitionsMapPtr _definitionsMap);
	virtual ~DefinitionResolver(void);

	//TypePtr getExpressionType(ExpressionPtr expression);
	//void setExpressionType(ExpressionPtr expression, TypePtr expressionType);
	ExpressionPtr getExpressionDefinition(const std::string name, int subscript);

private:
	definitionsMapPtr definitionsMap;
};

