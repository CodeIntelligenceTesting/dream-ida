/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include "IntermediateRepresentation/IRSharedPointers.h"
class Expression;


class Variable
{
public:
	Variable();
	Variable(std::string _name, int _subscript);
	Variable(std::string _name, int _subscript, int _phiSourceBlockId);
	virtual ~Variable(void);

	std::string toString() const;
	bool operator < (const Variable& var) const {return toString().compare(var.toString()) < 0;};
	bool operator == (const Variable& var) const {return toString().compare(var.toString()) == 0;};

	std::string name;
	int subscript;
	int phiSourceBlockId;
	bool resolved;
	bool isNullVariable();
	bool isEqual(const Variable &toThis);
	bool doesRepresentSameVariable(const ExpressionPtr &exp) const;
};

