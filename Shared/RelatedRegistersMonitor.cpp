////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "RelatedRegistersMonitor.h"

#include "../IntermediateRepresentation/Instruction/Instruction.h"
//#include "../IntermediateRepresentation/Expression/Expression.h"
#include "../DataFlowAnalysis/DataStructures/Variable.h"

RelatedRegistersMonitor::RelatedRegistersMonitor(definitionsMapPtr _definitionsMap, usesMapPtr _usesMap)
{
	definitionsMap = _definitionsMap;
	usesMap = _usesMap;
	constructRelatedRegistersMap();
}

RelatedRegistersMonitor::~RelatedRegistersMonitor(void)
{
}

//std::set<std::string>* RelatedRegistersMonitor::getRelatedRegisters(std::string registerName){
//	std::map<std::string, std::set<std::string>>::iterator reg_iter = relatedRegistersMap.find(registerName);
//	if(reg_iter != relatedRegistersMap.end()){
//		return &(reg_iter->second);
//	}
//	return NULL;
//}

bool RelatedRegistersMonitor::noRelatedRegisterUsed(const std::string &name, int subscript){
	InstructionPtr definingInstruction = definitionsMap->operator[](name).operator[](subscript);
	for(std::set<std::string>::iterator reg_iter = relatedRegistersMap[name].begin() ; reg_iter != relatedRegistersMap[name].end() ; ++reg_iter){
		for(std::map<int, InstructionPtr>::iterator subscript_iter = definitionsMap->operator[](*reg_iter).begin() ; subscript_iter != definitionsMap->operator[](*reg_iter).end() ; ++subscript_iter){
			if(definingInstruction == subscript_iter->second){
				//if(!(*usesMap)[*reg_iter][subscript_iter->first].empty()){
				if(!usesMap->operator[](*reg_iter).operator[](subscript_iter->first).empty()){
					return false;
				}
			}
		}
	}
	return true;
}

void RelatedRegistersMonitor::getDefinedRelatedRegister(const std::string &name, int subscript, Variable &relatedRegister_out){
	std::map<std::string, std::set<std::string>>::iterator relatedRegisters_iter = relatedRegistersMap.find(name);
	if(relatedRegisters_iter == relatedRegistersMap.end()){
		relatedRegister_out = Variable(name, subscript);
	}
	else{
		//Variable* relatedRegister = NULL;
		std::set<std::string>* relatedRegisters = &(relatedRegisters_iter->second);
		InstructionPtr definingInstruction = definitionsMap->operator[](name).operator[](subscript);
		std::vector<ExpressionPtr> definedElements;
		definingInstruction->getDefinedElements(definedElements);
		for(std::vector<ExpressionPtr>::iterator def_iter = definedElements.begin() ; def_iter != definedElements.end() ; ++def_iter){
			if(relatedRegisters->find((*def_iter)->getName()) != relatedRegisters->end()){
				relatedRegister_out = Variable((*def_iter)->getName(), (*def_iter)->subscript);
				return;
			}
		}
		relatedRegister_out = Variable(name, subscript);
	}
}

void RelatedRegistersMonitor::getUsedRelatedRegisters(const std::string &name, int subscript, std::vector<Variable> &relatedUses_out){
	//std::vector<Variable>* relatedUses = new std::vector<Variable>();
	relatedUses_out.push_back(Variable(name, subscript));
	InstructionPtr definingInstruction = definitionsMap->operator[](name).operator[](subscript);
	for(std::set<std::string>::iterator reg_iter = relatedRegistersMap[name].begin() ; reg_iter != relatedRegistersMap[name].end() ; ++reg_iter)
		for(std::map<int, InstructionPtr>::iterator subscript_iter = definitionsMap->operator[](*reg_iter).begin() ; subscript_iter != definitionsMap->operator[](*reg_iter).end() ; ++subscript_iter)
			if(definingInstruction == subscript_iter->second)
				if(!usesMap->operator[](*reg_iter).operator[](subscript_iter->first).empty())
					relatedUses_out.push_back(Variable(*reg_iter, subscript_iter->first));
}

void RelatedRegistersMonitor::constructRelatedRegistersMap(){
	relatedRegistersMap["eax"].insert("ax");
	relatedRegistersMap["eax"].insert("ah");
	relatedRegistersMap["eax"].insert("al");
	relatedRegistersMap["ax"].insert("eax");
	relatedRegistersMap["ax"].insert("ah");
	relatedRegistersMap["ax"].insert("al");
	relatedRegistersMap["ah"].insert("eax");
	relatedRegistersMap["ah"].insert("ax");
	relatedRegistersMap["al"].insert("eax");
	relatedRegistersMap["al"].insert("ax");

	relatedRegistersMap["ebx"].insert("bx");
	relatedRegistersMap["ebx"].insert("bh");
	relatedRegistersMap["ebx"].insert("bl");
	relatedRegistersMap["bx"].insert("ebx");
	relatedRegistersMap["bx"].insert("bh");
	relatedRegistersMap["bx"].insert("bl");
	relatedRegistersMap["bh"].insert("ebx");
	relatedRegistersMap["bh"].insert("bx");
	relatedRegistersMap["bl"].insert("ebx");
	relatedRegistersMap["bl"].insert("bx");

	
	relatedRegistersMap["ecx"].insert("cx");
	relatedRegistersMap["ecx"].insert("ch");
	relatedRegistersMap["ecx"].insert("cl");
	relatedRegistersMap["cx"].insert("ecx");
	relatedRegistersMap["cx"].insert("ch");
	relatedRegistersMap["cx"].insert("cl");
	relatedRegistersMap["ch"].insert("ecx");
	relatedRegistersMap["ch"].insert("cx");
	relatedRegistersMap["cl"].insert("ecx");
	relatedRegistersMap["cl"].insert("cx");

	
	relatedRegistersMap["edx"].insert("dx");
	relatedRegistersMap["edx"].insert("dh");
	relatedRegistersMap["edx"].insert("dl");
	relatedRegistersMap["dx"].insert("edx");
	relatedRegistersMap["dx"].insert("dh");
	relatedRegistersMap["dx"].insert("dl");
	relatedRegistersMap["dh"].insert("edx");
	relatedRegistersMap["dh"].insert("dx");
	relatedRegistersMap["dl"].insert("edx");
	relatedRegistersMap["dl"].insert("dx");

	relatedRegistersMap["esi"].insert("si");
	relatedRegistersMap["si"].insert("esi");

	relatedRegistersMap["edi"].insert("di");
	relatedRegistersMap["di"].insert("edi");

	relatedRegistersMap["esp"].insert("sp");
	relatedRegistersMap["sp"].insert("esp");

	relatedRegistersMap["edp"].insert("dp");
	relatedRegistersMap["dp"].insert("edp");
}
