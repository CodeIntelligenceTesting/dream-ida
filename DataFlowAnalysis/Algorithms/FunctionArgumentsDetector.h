/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "DataFlowAlgorithm.h"
#include "../../IntermediateRepresentation/IRSharedPointers.h"

class FunctionArgumentsDetector :
	public DataFlowAlgorithm
{
public:
	FunctionArgumentsDetector(ControlFlowGraphPtr _controlFlowGraph,
							  definitionsMapPtr _definitionsMap,
							  usesMapPtr _usesMap);
	virtual ~FunctionArgumentsDetector(void);

	ExpressionVectorPtr getArguments();
private:
	bool isArgument(const ExpressionPtr &exp);
	bool isOnlyUsedinAddressExpressions(const ExpressionPtr &exp);
};

