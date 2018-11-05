/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <vector>

//To be deleted
#include <ida.hpp>
#include <idp.hpp>

#include "NodeTypes.h"
#include "../Instruction/Instruction.h"

class Node
{
public:
	Node(int _id, NodeType _nodeType, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions);
	virtual ~Node(void);

	char* getInstructionsCOLSTR();
	std::string getNodeCOLSTR();
	virtual intSetPtr getSuccessors(){return std::make_shared<std::set<int>>();};
	intSetPtr getPredecessors(){return predecessors;};
	int id;
	NodeType nodeType;
	ea_t startEA;
	ea_t endEA;
	InstructionVectorPtr instructions;
	void addAfterInstruction(InstructionPtr new_inst, InstructionPtr after_this);

	std::string instructions_json();

	virtual NodePtr deepCopy() = 0;
	InstructionVectorPtr getInstructionsCopy();
//private:
	intSetPtr predecessors;
	void removePredecessor(int predId);
};

