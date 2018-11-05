/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../IntermediateRepresentation/IRSharedPointers.h"

//class Expression;
class Instruction;
class Variable;

class RelatedRegistersMonitor
{
public:
	RelatedRegistersMonitor(definitionsMapPtr _definitionsMap, usesMapPtr _usesMap);
	virtual ~RelatedRegistersMonitor(void);

	//std::set<std::string>* getRelatedRegisters(std::string registerName);
	bool noRelatedRegisterUsed(const std::string &name, int subscript);
	void getDefinedRelatedRegister(const std::string &name, int subscript, Variable &relatedRegister_out);
	void getUsedRelatedRegisters(const std::string &name, int subscript, std::vector<Variable> &relatedUses_out);

private:
	definitionsMapPtr definitionsMap;
	usesMapPtr usesMap;
	std::map<std::string, std::set<std::string>> relatedRegistersMap;
	void constructRelatedRegistersMap();
};

