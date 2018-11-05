/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "../TypeSharedPointers.h"
#include "../TypeMap.h"

class TypeAnalyzer
{
public:
	TypeAnalyzer(definitionsMapPtr _definitionsMap, usesMapPtr _usesMap);
	virtual ~TypeAnalyzer(void);

	void analyzeTypes();
	TypeMapPtr getTypeMap(){return typeMap;};
private:
	definitionsMapPtr definitionsMap;
	usesMapPtr usesMap;
	TypeMapPtr typeMap;

	int tmp;

	//void unifyUsesAndDefinitionsTypes();

	void propagateTypeToDefiningInstruction(ExpressionPtr definedExpression, InstructionPtr definingInstruction);
	void propagateTypeToUsingInstructions(ExpressionPtr usedExpression);
	void analyzeTypesInAssignment(AssignmentPtr assignment);
	void setDefTypeFromDefiningInstruction(InstructionPtr definingInstruction);
	void updateExpressionType(ExpressionPtr expression, TypePtr type);
	//void updateDefinitionType(ExpressionPtr expression, TypePtr type);
	//void updateUsesType(ExpressionPtr expression, TypePtr type);
	void propagateTypeToPhiFunction(Phi_FunctionPtr phiFunction, TypePtr type);

	TypePtr getTypeFromAddressExpression(AddressExpressionPtr addressExpression);
	TypePtr getTypeFromAdditionExpression(AdditionExpressionPtr additionExpression);
	bool isDefined(ExpressionPtr exp);
	//bool isUsed(ExpressionPtr exp);
	void getExpressionUses(ExpressionPtr exp, std::vector<ExpressionPtr>& uses);
	bool isNewTypeMoreSpecific(TypePtr currentType, TypePtr newType);
	void setExpressionsType(std::vector<ExpressionPtr>& expressions, TypePtr type);

	void constructTypeMap();
	TypePtr getTypeFromUses(const std::string& name, int subscript);
};

