////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DataFlowAlgorithm.h"



DataFlowAlgorithm::DataFlowAlgorithm(ControlFlowGraphPtr _controlFlowGraph,
									 definitionsMapPtr _definitionsMap,
									 definitionsMapPtr _flagDefinitionsMap,
									 usesMapPtr _usesMap)
{
	controlFlowGraph = _controlFlowGraph;
	definitionsMap = _definitionsMap;
	flagDefinitionsMap = _flagDefinitionsMap;
	usesMap = _usesMap;
	constructRelatedRegistersMap();
}


DataFlowAlgorithm::~DataFlowAlgorithm(void)
{
}

//std::vector<Expression*>* DataFlowAlgorithm::getFunctionArguments(){
//	std::vector<Expression*>* arguments = new std::vector<Expression*>();
//	std::set<std::string> foundArgumentsNames;
//	for(unsigned int nodeId = 0 ; nodeId < controlFlowGraph->nodes->size() ; ++nodeId){
//		std::vector<Instruction*>* nodeInstructions = (*(controlFlowGraph->nodes))[nodeId]->instructions;
//		for(std::vector<Instruction*>::iterator inst_iter = nodeInstructions->begin() ; inst_iter != nodeInstructions->end() ; ++inst_iter){
//			Instruction* currentInstruction = *inst_iter;
//			if(currentInstruction->type != PHI_FUNCTION){
//				std::vector<Expression*>* usedElements = currentInstruction->getUsedElements();
//				for(std::vector<Expression*>::iterator use_iter = usedElements->begin() ; use_iter != usedElements->end() ; ++use_iter){
//					Expression* usedElement = *use_iter;
//					if(usedElement->subscript == NO_SUBSCRIPT || usedElement->subscript == 0){
//						if(!usedElement->getName().empty() && foundArgumentsNames.find(usedElement->getName()) == foundArgumentsNames.end()){
//							if(usedElement->type != POINTER && usedElement->type != GLOBAL_VARIABLE){
//								if(usedElement->type != REGISTER){
//									foundArgumentsNames.insert(usedElement->getName());
//									arguments->push_back(usedElement);
//								}
//								else if(((Register*)usedElement)->regNo != R_sp){
//									foundArgumentsNames.insert(usedElement->getName());
//									arguments->push_back(usedElement);
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	return arguments;
//}

void DataFlowAlgorithm::addInstructionToUsesMap(const ExpressionPtr& usedVariable, InstructionPtr instructionToAdd){
	usesMap->operator[](usedVariable->getName()).operator[](usedVariable->subscript).push_back(instructionToAdd);
	//(*usesMap)[usedVariable->getName()][usedVariable->subscript].push_back(instructionToAdd);
}

void DataFlowAlgorithm::deleteInstructionFromUsesMap(const ExpressionPtr& unusedVariable, InstructionPtr instructionToRemove){
	removeInstructionFromVariableUsesMap(unusedVariable->getName(), unusedVariable->subscript, instructionToRemove);
}

void DataFlowAlgorithm::updateDefinitionsMap(const ExpressionPtr& definedVariable, InstructionPtr definingInstruction){
	definitionsMap->operator[](definedVariable->getName()).operator[](definedVariable->subscript) = definingInstruction;
	//(*definitionsMap)[definedVariable->getName()][definedVariable->subscript] = definingInstruction;
}

void DataFlowAlgorithm::constructRelatedRegistersMap(){
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

bool DataFlowAlgorithm::noRelatedRegisterUsed(const std::string& name, int subscript){
	/*if((name.compare("edx") == 0 || name.compare("dx") == 0) && subscript == 26)
		msg("hi\n");*/
	if(relatedRegistersMap.find(name) != relatedRegistersMap.end()){
		InstructionPtr definingInstruction = getDefiningInstruction(name, subscript);//(*definitionsMap)[name][subscript];
		for(std::set<std::string>::iterator reg_iter = relatedRegistersMap[name].begin() ; reg_iter != relatedRegistersMap[name].end() ; ++reg_iter){
			//for(std::map<int, InstructionPtr>::iterator subscript_iter = (*definitionsMap)[*reg_iter].begin() ; subscript_iter != (*definitionsMap)[*reg_iter].end() ; ++subscript_iter){
			for(std::map<int, InstructionPtr>::iterator subscript_iter = definitionsMap->operator[](*reg_iter).begin() ; subscript_iter != definitionsMap->operator[](*reg_iter).end() ; ++subscript_iter){
				if(definingInstruction == subscript_iter->second){
					bool isRelatedRegisterNotUsed = usesMap->operator[](*reg_iter).operator[](subscript_iter->first).empty();
					if(!isRelatedRegisterNotUsed/*(*usesMap)[*reg_iter][subscript_iter->first].empty()*/){
						return false;
					}
				}
			}
		}
	}
	return true;
}

void DataFlowAlgorithm::removeInstructionFromVariableUsesMap(const std::string& variableName, int variableSubscript, const InstructionPtr &instructionToRemove){
	std::vector<InstructionPtr>* usingInstructionsList = &(usesMap->operator[](variableName).operator[](variableSubscript));//&((*usesMap)[variableName][variableSubscript]);
	std::vector<InstructionPtr>::iterator removeInst_iter = std::find(usingInstructionsList->begin(), usingInstructionsList->end(), instructionToRemove);
	if(removeInst_iter != usingInstructionsList->end()){
		usingInstructionsList->erase(removeInst_iter);
	}
}

bool DataFlowAlgorithm::isVariableDead(const std::string& variableName, int variableSubscript){
	return usesMap->operator[](variableName).operator[](variableSubscript).empty() && noRelatedRegisterUsed(variableName, variableSubscript);
	//return (*usesMap)[variableName][variableSubscript].empty() && noRelatedRegisterUsed(variableName, variableSubscript);
}

bool DataFlowAlgorithm::isVariableDefined(const std::string& variableName, int variableSubscript){
	return definitionsMap->find(variableName) != definitionsMap->end() && variableSubscript != NO_SUBSCRIPT;
}

InstructionPtr DataFlowAlgorithm::getDefiningInstruction(const std::string& variableName, int variableSubscript){
	if(definitionsMap->find(variableName) != definitionsMap->end()){
		if(definitionsMap->operator[](variableName).find(variableSubscript) != definitionsMap->operator[](variableName).end()){
			return definitionsMap->operator[](variableName).operator[](variableSubscript);
		}
	}
	InstructionPtr nullInstruction = std::shared_ptr<Instruction>();
	return nullInstruction;
}

std::vector<InstructionPtr>* DataFlowAlgorithm::getUsingInstructions(const std::string& variableName, int variableSubscript){
	return &(usesMap->operator[](variableName).operator[](variableSubscript));
	//return &((*usesMap)[variableName][VariableSubscript]);
}

std::vector<InstructionPtr>* DataFlowAlgorithm::getUsingInstructions(const ExpressionPtr& variable){
	return &(usesMap->operator[](variable->getName()).operator[](variable->subscript));
	//return &((*usesMap)[variable->getName()][variable->subscript]);
}


bool DataFlowAlgorithm::isInExpressionList(std::vector<ExpressionPtr> &expr_list, const ExpressionPtr& expr){
	for(std::vector<ExpressionPtr>::iterator iter = expr_list.begin() ; iter != expr_list.end() ; ++iter){
		if((*iter)->isSameVariable(expr->getName(), expr->subscript))
			return true;
	}
	return false;
}
//void DataFlowAlgorithm::clearUses(Expression* exp){
//	std::vector<Instruction*>* usingInstructions = getUsingInstructions(exp->getName(), exp->subscript);
//	usingInstructions->clear();
//}


//Instruction* DataFlowAlgorithm::getDefiningInstruction(std::string variableName, int VariableSubscript){
//	return (*definitionsMap)[variableName][VariableSubscript];
//}
//
//std::vector<Instruction*>* DataFlowAlgorithm::getUsingInstructions(std::string variableName, int VariableSubscript){
//	return &((*usesMap)[variableName][VariableSubscript]);
//}
