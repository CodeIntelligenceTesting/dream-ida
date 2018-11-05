////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "FunctionArgumentsDetector.h"

#include "../../IntermediateRepresentation/ControlFlowGraph.h"

FunctionArgumentsDetector::FunctionArgumentsDetector(ControlFlowGraphPtr _controlFlowGraph,
													 definitionsMapPtr _definitionsMap,
													 usesMapPtr _usesMap)
													 : DataFlowAlgorithm(_controlFlowGraph, _definitionsMap, std::shared_ptr<std::map<std::string, std::map<int, InstructionPtr>>>(), _usesMap)
{
}


FunctionArgumentsDetector::~FunctionArgumentsDetector(void)
{
}

ExpressionVectorPtr FunctionArgumentsDetector::getArguments(){
	ExpressionVectorPtr functionArgumrnts = std::make_shared<std::vector<ExpressionPtr>>();
	std::set<std::string> addedElements;

	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		InstructionVectorPtr instructions = node_iter->second->instructions;
		for(std::vector<InstructionPtr>::iterator inst_iter = instructions->begin() ; inst_iter != instructions->end() ; ++inst_iter){
			InstructionPtr currentInstruction = *inst_iter;
			std::vector<ExpressionPtr> usedElements;
			currentInstruction->getUsedElements(usedElements);
			for(std::vector<ExpressionPtr>::iterator use_iter = usedElements.begin() ; use_iter != usedElements.end() ; ++use_iter){
				ExpressionPtr usedElement = *use_iter;
				std::string usedElementString = usedElement->getExpressionString();
				if(isArgument(usedElement) && addedElements.find(usedElementString) == addedElements.end()){
					functionArgumrnts->push_back(usedElement);
					addedElements.insert(usedElementString);
				}
			}
		}
	}
	return functionArgumrnts;
}

bool FunctionArgumentsDetector::isArgument(const ExpressionPtr &exp){
	return (exp->subscript == NO_SUBSCRIPT || exp->subscript == 0)
			&& !exp->getName().empty()
			&& exp->type != POINTER && exp->type != GLOBAL_VARIABLE
			&& (exp->type != REGISTER ? true : std::dynamic_pointer_cast<Register>(exp)->regNo != R_sp); 
}

bool FunctionArgumentsDetector::isOnlyUsedinAddressExpressions(const ExpressionPtr &exp){
	return false;
}
