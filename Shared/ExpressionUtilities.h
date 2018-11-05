/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <vector>
#include "../IntermediateRepresentation/IRSharedPointers.h"

void createExpressionElements(const ExpressionPtr& rootExpressionPtr, bool includePointers, std::vector<ExpressionPtr>& elements_out);
void addRootElement(const ExpressionPtr& rootExpressionPtr, bool includePointers, std::vector<ExpressionPtr>& elements_out);
void addElementIfSimple(const ExpressionPtr& rootExpressionPtr, std::vector<ExpressionPtr>& elements_out);
void addElementIfMemoryVariable(const ExpressionPtr& rootExpressionPtr, std::vector<ExpressionPtr>& elements_out);

bool isExpressionPointerNull(const ExpressionPtr& expPtr);

ExpressionPtr getNegatedExpression(ExpressionPtr &booleanExpression);
ANDExpressionPtr getCompundCondition_AND(ExpressionPtr &conditionA, ExpressionPtr &conditionB);
ORExpressionPtr getCompundCondition_OR(ExpressionPtr &conditionA, ExpressionPtr &conditionB);
std::string getNegatedOperator(std::string conditionalOperator);
