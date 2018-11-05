/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <set>
#include <string>
//#include <vector>

#include "FlagMacroTypes.h"
#include "../Instruction.h"
#include "../../Enumerations/Flags.h"
#include "../../Expression/Flag.h"
#include "../../../Shared/ExpressionUtilities.h"


class FlagMacro :
	public Instruction
{
public:
	FlagMacro(FlagMacroType _flagMacroType);
	virtual ~FlagMacro(void);

	char* getColoredInstructionString();
	std::string getInstructionCOLSTR();
	//std::vector<Expression*>* getDefinedElements();
	void getDefinedElements(std::vector<ExpressionPtr>& definedElements_out);

	bool replaceDefinition(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression);

	//FlagSetPtr getDefinedFlagsCopy();


	FlagSetPtr definedFlags;
	FlagMacroType flagMacroType;
};

