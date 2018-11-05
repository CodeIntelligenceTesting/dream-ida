/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <string>
#include <vector>

#include <idp.hpp>

#include "Condition.h"
#include "../../Enumerations/BranchTypes.h"
//#include "../../../IntermediateRepresentation/IRSharedPointers.h"
class Flag;

class LowLevelCondition :
	public Condition
{
public:
	LowLevelCondition(FlagVectorPtr _testedFlags, BranchType _branchType);
	virtual ~LowLevelCondition(void);

	std::string getExpressionString();
	char* getColoredExpressionString();
	std::string getExpressionCOLSTR();
	//std::vector<Expression*>* getExpressionElements(bool includePointers);
	void getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out);
	ExpressionPtr deepcopy();
	bool replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);
	bool isUnsignedCompare();

	FlagVectorPtr testedFlags;
	BranchType branchType;
private:
	std::string getBranchTypeString();
};

