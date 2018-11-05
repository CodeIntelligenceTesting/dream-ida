////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Node.h"

#include <boost/lexical_cast.hpp>
//#include "../../Shared/houseKeepingUtilities.h"

Node::Node(int _id, NodeType _nodeType, ea_t _startEA, ea_t _endEA, InstructionVectorPtr _instructions)
{
	id = _id;
	nodeType = _nodeType;
	startEA = _startEA;
	endEA = _endEA;
	instructions = _instructions;
	predecessors = std::make_shared<std::set<int>>();
}


Node::~Node(void)
{
	//msg("deleting Node %d (%d instructions)\n", id, instructions->size());
	//deleteVectorPointer(instructions); instructions = NULL;
}

char* Node::getInstructionsCOLSTR(){
	if(instructions->empty())
		return NULL;
	std::vector<char*> instCOLSTR;
	int inst_num = instructions->size();
	int* instCOLSTRSize = new int[inst_num];
	int nodeCOLSTRSize = inst_num - 1;
	
	for(int i = 0 ; i < inst_num ; i++){
		//msg("%d: %s\n", id, (*instructions)[i]->getInstructionString().c_str());
		char* currentInstCOLSTR = (*instructions)[i]->getColoredInstructionString();
		instCOLSTRSize[i] = strlen(currentInstCOLSTR);
		nodeCOLSTRSize += instCOLSTRSize[i];
		instCOLSTR.push_back(currentInstCOLSTR);
	}

	char* nodeCOLSTR = (char*)malloc(nodeCOLSTRSize + 1);
	int currentPosition = 0;
	for(int i = 0 ; i < inst_num ; i++){
		memcpy(nodeCOLSTR + currentPosition, instCOLSTR[i], instCOLSTRSize[i]);
		currentPosition += instCOLSTRSize[i];
		if(i != inst_num - 1){
			memcpy(nodeCOLSTR + currentPosition, "\n", 1);
			currentPosition++;
		}
		free(instCOLSTR[i]);
	}
	memcpy(nodeCOLSTR + currentPosition, "\0", 1);
	return nodeCOLSTR;
	
//	////////////////////////////////////////////////
//	char* newLine = "\n";
//	int newLineSize = strlen(newLine);
//
//	//int inst_num = instructions->size();
////	int nodeCOLSTRSize = inst_num * (newLineSize - 1);
//	//char** instCOLSTR = new char*[inst_num];
//	//int* instCOLSTRSize = new int[inst_num];
//	msg("COLSTR for node: %d\n", id);
//	for(int i = 0 ; i < inst_num ; i++){
//		//msg("Instruction(%d): %s\n", i, (*instructions)[i]->getInstructionString().c_str());
//		instCOLSTR[i] = (*instructions)[i]->getColoredInstructionString();
//		instCOLSTRSize[i] = strlen(instCOLSTR[i]);
//		nodeCOLSTRSize += instCOLSTRSize[i];
//		msg("instCOLSTRSize[i]=%d\n", instCOLSTRSize[i]);
//	}
//	msg("DONE: COLSTR for individual instructions of node: %d\n", id);
//	//char* nodeCOLSTR = (char*)malloc(nodeCOLSTRSize + 1);
//	//int currentPosition = 0;
//	for(int i = 0 ; i < inst_num ; i++){
//		memcpy(nodeCOLSTR + currentPosition, instCOLSTR[i], instCOLSTRSize[i]);
//		currentPosition += instCOLSTRSize[i];
//		if(i != inst_num - 1){
//			memcpy(nodeCOLSTR + currentPosition, newLine, newLineSize);
//			currentPosition += newLineSize;
//		}
//		//free(instCOLSTR[i]);
//	}
//	memcpy(nodeCOLSTR + currentPosition, "\0", 1);
//
//	//delete[] instCOLSTR;
//	delete[] instCOLSTRSize;
//	msg("DONE: COLSTR of node: %d\n", id);
//	return nodeCOLSTR;
}

std::string Node::getNodeCOLSTR(){
	std::string coloredString = "id(" + boost::lexical_cast<std::string>(id) + "), type(" + boost::lexical_cast<std::string>(nodeType) + ")\n";
	coloredString.append("predecessors: ");
	for(std::set<int>::iterator pred_iter = predecessors->begin() ; pred_iter != predecessors->end() ; ++pred_iter)
		coloredString.append(boost::lexical_cast<std::string>(*pred_iter) + ", ");
	coloredString.append("\n");

	if(instructions == NULL || instructions->empty())
		return coloredString + "EMPTY";
	for(std::vector<InstructionPtr>::iterator inst_iter = instructions->begin() ; inst_iter != instructions->end() ; ++inst_iter){
		coloredString.append((*inst_iter)->getInstructionCOLSTR());
		if(inst_iter + 1 != instructions->end())
			coloredString.append("\n");
	}
	return coloredString;
}

InstructionVectorPtr Node::getInstructionsCopy(){
	InstructionVectorPtr instructionsCopy = std::make_shared<std::vector<InstructionPtr>>();
	for(std::vector<InstructionPtr>::iterator inst_iter = instructions->begin() ; inst_iter != instructions->end() ; ++inst_iter){
		instructionsCopy->push_back((*inst_iter)->deepcopy());
	}
	return instructionsCopy;
}

void Node::removePredecessor(int predId){
	//msg("removePredecessor(%d)\n", predId);
	//msg("predecessors: ");
	//for(std::set<int>::iterator it = predecessors->begin() ; it != predecessors->end() ; ++it)
	//	msg("%d, ", *it);
	//msg("\n");
	predecessors->erase(predId);
}

void Node::addAfterInstruction(InstructionPtr new_inst, InstructionPtr after_this){
	std::vector<InstructionPtr>::iterator pos = std::find(instructions->begin(), instructions->end(), after_this);
	instructions->insert(pos, new_inst);
	new_inst->ContainingNodeId = id;
}

std::string Node::instructions_json(){
	std::string json_string = "\"instructions\":[";
	if(instructions != NULL || !instructions->empty())
		for(std::vector<InstructionPtr>::iterator inst_iter = instructions->begin() ; inst_iter != instructions->end() ; ++inst_iter){
			InstructionPtr instruction = *inst_iter;
			if(instruction->type != CONDITIONAL_JUMP && instruction->type != DIRECT_JUMP && instruction->type != INDIRECT_JUMP && instruction->type != INDIRECT_TABLE_JUMP){
				json_string.append(instruction->to_json());
				if(inst_iter + 1 != instructions->end())
					json_string.append(", ");
			}
		}
	if(json_string.at(json_string.length() - 1) == ' ' && json_string.at(json_string.length() - 2) == ',')
		json_string = json_string.substr(0, json_string.length() - 2);
	json_string.append("]");
	return json_string;
}
