////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "DecompiledCodeGenerator.h"

#include "IntermediateRepresentation/ControlFlowGraph.h"
#include "ControlFlowAnalysis/GraphStructeringInfo.h"

#include "../Shared/ColoredStrings.h"
#include "../IntermediateRepresentation/Enumerations/CodeColors.h"

#include "../ControlFlowAnalysis/GraphUtilities/GraphConverter.h"
#include "../ControlFlowAnalysis/GraphTraversal/GraphPaths.h"
#include <boost/lexical_cast.hpp>
#include <lines.hpp>

#include "../DataFlowAnalysis/DataStructures/Variable.h"
#include "../IntermediateRepresentation/Enumerations/CodeColors.h"

//#define FUNCTION_COLOR SCOLOR_IMPNAME
//#define STRING_COLOR SCOLOR_ALTOP
//#define CONDITIONAL_COLOR SCOLOR_MACRO
////#define NUMBER_COLOR SCOLOR_CHAR //SCOLOR_SEGNAME
//#define VARIABLE_COLOR SCOLOR_INSN //SCOLOR_CHAR


DecompiledCodeGenerator::DecompiledCodeGenerator(ControlFlowGraphPtr _controlFlowGraph,
							GraphStructeringInfoPtr _graphStructeringInfo,
							ExpressionVectorPtr _functionArguments,
							std::string _functionName)
{
	controlFlowGraph = _controlFlowGraph;
	GraphConverter graphConverter;
	graphConverter.convertToBoostGraph(controlFlowGraph, graph);
	graphStructeringInfo = _graphStructeringInfo;
	functionArguments = _functionArguments;
	functionName = _functionName;
	codeLines = std::make_shared<std::vector<std::string>>();
	currentLoopHeaderId = -1;
	numberOfGotos = 0;
}


DecompiledCodeGenerator::~DecompiledCodeGenerator(void)
{
}

void DecompiledCodeGenerator::generateCodeLines(TypeMapPtr typeMap){
	std::string functionHeader = getCOLSTR(functionName, DECOMPILED_FUNCTION_COLOR) + "(";
	for(std::vector<ExpressionPtr>::iterator arg_iter = functionArguments->begin() ; arg_iter != functionArguments->end() ; ++arg_iter){
		TypePtr argType = (*arg_iter)->getExpresstionType(typeMap);
		if(!argType->isVoidType())
			functionHeader += argType->getTypeCOLSTR()/* + " "*/;
		functionHeader += (*arg_iter)->getExpressionCOLSTR();
		if(arg_iter + 1 != functionArguments->end())
			functionHeader += ", ";
	}
	functionHeader += ") {";
	codeLines->push_back(functionHeader);

	std::set<Variable> handledVariables;
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		for(std::vector<InstructionPtr>::iterator inst_iter = currentNode->instructions->begin() ; inst_iter != currentNode->instructions->end() ; ++inst_iter){
			std::vector<ExpressionPtr> definedElements;
			(*inst_iter)->getDefinedElements(definedElements);
			for(std::vector<ExpressionPtr>::iterator def_iter = definedElements.begin() ; def_iter != definedElements.end() ; ++def_iter){
				ExpressionPtr definedElement = *def_iter;
				Variable definedVariable(definedElement->getName(), definedElement->subscript);
				if(definedElement->type != POINTER && definedElement->expressionType->type != TOP_TYPE && handledVariables.find(definedVariable) == handledVariables.end()){
					handledVariables.insert(definedVariable);
					codeLines->push_back(getIndent(1) + definedElement->expressionType->getTypeCOLSTR() + " " + definedElement->getExpressionCOLSTR() + ";");
				}
			}
		}
	}
	codeLines->push_back("\n");
	
	writeCode(0, 1, -1, -1);
	codeLines->push_back("}");
}

void DecompiledCodeGenerator::writeCode(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId){
	msg("writeCode(%d, %d, %d, %d)\n", basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	if(handledNodeIds.find(basicBlockId) != handledNodeIds.end() || basicBlockId == ifFollowNodeId)
		return;
	handledNodeIds.insert(basicBlockId);
	lineNumberToLabelMap.insert(std::pair<int, int>(codeLines->size(), basicBlockId));

	if(isSwitchHeader(basicBlockId)){
		writeNWay(basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	if(isLoopHeader(basicBlockId))
		writeLoop(basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	/*else if(isBreakNode(basicBlockId)){
		writeBreakNode(basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	else if(isGotoNode(basicBlockId)){
		writeGotoNode();
	}*/
	else if(isConditionalHeader(basicBlockId))
		writeTwoWay(basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	else{
		NodeType nodeType = controlFlowGraph->nodes->at(basicBlockId)->nodeType;
		if(basicBlockId == latchingNodeId || nodeType == RETURN_NODE || nodeType == Exit_NODE)
			writeBasicBlock(basicBlockId, indentLevel);
		else if(nodeType == ONE_WAY_NODE)
			writeOneWay(basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	}
}

void DecompiledCodeGenerator::writeLoop(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId){
	msg("writeLoop(%d, %d, %d, %d)\n", basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	
	bool handleHeaderSuccessors = true;
	int previousLoopHeaderId = currentLoopHeaderId;
	currentLoopHeaderId = basicBlockId;
	
	handledNodeIds.insert(basicBlockId);	
	NodePtr headerNode = controlFlowGraph->nodes->at(basicBlockId);
	const Loop &loop = graphStructeringInfo->loops[basicBlockId];
	if(loop.loopType == PRE_TESTED){
		writeBasicBlock(basicBlockId, indentLevel);
		TwoWayNodePtr headerTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(headerNode);
		ExpressionPtr loopCondition = std::dynamic_pointer_cast<ConditionalJump>(headerTwoWayNode->instructions->back())->condition;
		if(headerTwoWayNode->trueSuccessorID == loop.followNodeId)
			loopCondition = getNegatedExpression(loopCondition);
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("while", CONDITIONAL_COLOR)) + "("
			+ loopCondition->getExpressionCOLSTR() + ") {");
	}
	else if(loop.loopType == POST_TESTED || loop.loopType == ENDLESS){
		if(loop.loopType == POST_TESTED)
			codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("do", CONDITIONAL_COLOR)) + "{");
		else
			codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("while", CONDITIONAL_COLOR))
								 + "(" + std::string(COLSTR("true", NUMBER_COLOR)) + ") {");

		if(headerNode->nodeType == TWO_WAY_NODE){
			if(headerNode->id != loop.latchingNodeId){
				//writeTwoWay(headerNode->id, indentLevel + 1, loop.latchingNodeId, ifFollowNodeId);
				TwoWayNodePtr headerTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(headerNode);
				NodePtr trueSuccessorNode = controlFlowGraph->nodes->at(headerTwoWayNode->trueSuccessorID);
				NodePtr falseSuccessorNode = controlFlowGraph->nodes->at(headerTwoWayNode->falseSuccessorID);
				ExpressionPtr ifCondition = std::dynamic_pointer_cast<ConditionalJump>(headerTwoWayNode->instructions->back())->condition;
				if(trueSuccessorNode->nodeType == Exit_NODE){
					writeBasicBlock(headerTwoWayNode->id, indentLevel + 1);
					codeLines->push_back(getIndent(indentLevel + 1) + std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + ifCondition->getExpressionCOLSTR() + ") {");
					writeCode(trueSuccessorNode->id, indentLevel + 2, latchingNodeId, ifFollowNodeId);
					codeLines->push_back(getIndent(indentLevel + 1)+ "}");
				}
				else if(falseSuccessorNode->nodeType == Exit_NODE){
					writeBasicBlock(headerTwoWayNode->id, indentLevel + 1);
					codeLines->push_back(getIndent(indentLevel + 1) + std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ") {");
					writeCode(falseSuccessorNode->id, indentLevel + 2, latchingNodeId, ifFollowNodeId);
					codeLines->push_back(getIndent(indentLevel + 1)+ "}");
				}
				else if (trueSuccessorNode->id == loop.followNodeId){
					writeBasicBlock(headerTwoWayNode->id, indentLevel + 1);
					codeLines->push_back(getIndent(indentLevel + 1) + std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + ifCondition->getExpressionCOLSTR() + ")");
					codeLines->push_back(getIndent(indentLevel + 2) + std::string(COLSTR("break;", CODE_COLOR)));
				}
				else if(falseSuccessorNode->id == loop.followNodeId){
					writeBasicBlock(headerTwoWayNode->id, indentLevel + 1);
					codeLines->push_back(getIndent(indentLevel + 1) + std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ")");
					codeLines->push_back(getIndent(indentLevel + 2) + std::string(COLSTR("break;", CODE_COLOR)));
				}
				else if(graphStructeringInfo->conditionals.find(headerNode->id) != graphStructeringInfo->conditionals.end()){
					writeTwoWay(headerNode->id, indentLevel + 1, loop.latchingNodeId, ifFollowNodeId);
					handleHeaderSuccessors = false;
				}
				else{
					std::vector<int> trueSuccToHeadPathVertices, falseSuccToHeadPathVertices;
					if(getPathVertices(boost::vertex(headerTwoWayNode->trueSuccessorID, graph), boost::vertex(headerTwoWayNode->id, graph), trueSuccToHeadPathVertices, graph)
						&& getPathVertices(boost::vertex(headerTwoWayNode->falseSuccessorID, graph), boost::vertex(headerTwoWayNode->id, graph), falseSuccToHeadPathVertices, graph)){
							int commonNodeId = getCommonNodeId(trueSuccToHeadPathVertices, falseSuccToHeadPathVertices);
							//commonNodeId = commonNodeId == -1 ? ifFollowNodeId : commonNodeId;
							Conditional unstructuredConditional(headerTwoWayNode->id, commonNodeId, UNKNOWN_CONDITIONAL);
							graphStructeringInfo->conditionals.insert(std::pair<int, Conditional>(headerTwoWayNode->id, unstructuredConditional));
							//handledNodeIds.erase(handledNodeIds.find(basicBlockId));
							writeTwoWay(headerTwoWayNode->id, indentLevel + 1, loop.latchingNodeId, ifFollowNodeId);
							handleHeaderSuccessors = false;
					}
				}
			}
			else
				writeBasicBlock(basicBlockId, indentLevel + 1);
		}
		else{
			writeBasicBlock(basicBlockId, indentLevel + 1);
		}
	}

	if(basicBlockId == latchingNodeId)
		return;

	if(handleHeaderSuccessors && basicBlockId != loop.latchingNodeId){
		OutEdgeIterator e, end;
		IndexMap index = boost::get(boost::vertex_index, graph);
		for(boost::tie(e, end) = boost::out_edges(boost::vertex(basicBlockId, graph), graph) ; e != end ; ++e){
			int successorId = index[boost::target(*e, graph)];
			//test
			if(loop.loopNodes->find(successorId) != loop.loopNodes->end()){
				if(loop.loopType != PRE_TESTED || successorId != loop.followNodeId){
					if(handledNodeIds.find(successorId) == handledNodeIds.end())
						writeCode(successorId, indentLevel + 1 , loop.latchingNodeId, ifFollowNodeId);
					else
						emitGotoLabel(successorId, indentLevel + 1);
				}
			}
		}
	}

	if(loop.loopType == PRE_TESTED){
		writeBasicBlock(basicBlockId, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel) + "}");
	}
	else if(loop.loopType == POST_TESTED){
		if(!isNodeHandled(loop.latchingNodeId))
			writeBasicBlock(loop.latchingNodeId, indentLevel + 1);
		TwoWayNodePtr latchingTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(loop.latchingNodeId));
		ExpressionPtr loopCondition = std::dynamic_pointer_cast<ConditionalJump>(latchingTwoWayNode->instructions->back())->condition;
		if(latchingTwoWayNode->falseSuccessorID == loop.headerNodeId)
			loopCondition = getNegatedExpression(loopCondition);
		codeLines->push_back(getIndent(indentLevel) + "} " + std::string(COLSTR("while", CONDITIONAL_COLOR))
							 + "(" + loopCondition->getExpressionCOLSTR() + ");");
	}
	else if(loop.loopType == ENDLESS){
		codeLines->push_back(getIndent(indentLevel) + "}");
	}
	currentLoopHeaderId = previousLoopHeaderId;
	if(loop.followNodeId != -1){
		if(handledNodeIds.find(loop.followNodeId) == handledNodeIds.end())
			writeCode(loop.followNodeId, indentLevel, latchingNodeId, ifFollowNodeId);
		else
			emitGotoLabel(loop.followNodeId, indentLevel);
	}
}

void DecompiledCodeGenerator::writeNWay(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId){
	msg("writeNWay(%d, %d, %d, %d)\n", basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	writeBasicBlock(basicBlockId, indentLevel);
	IndirectTableJumpPtr switchInstruction = std::dynamic_pointer_cast<IndirectTableJump>(controlFlowGraph->nodes->at(basicBlockId)->instructions->back());
	codeLines->push_back(getIndent(indentLevel) + switchInstruction->getInstructionCOLSTR() + "{");
	for(std::map<int, std::set<int>>::iterator node_iter = switchInstruction->nodeToCasesMap->begin() ; node_iter != switchInstruction->nodeToCasesMap->end() ; ++node_iter){
		if(node_iter->first != switchInstruction->defaultBlockId && !node_iter->second.empty()){
			for(std::set<int>::iterator case_iter = node_iter->second.begin() ; case_iter != node_iter->second.end() ; ++case_iter){
				std::string caseCOLSTR = getCOLSTR(boost::lexical_cast<std::string>(*case_iter), NUMBER_COLOR);
				codeLines->push_back(getIndent(indentLevel + 1) + getCOLSTR("case", CONDITIONAL_COLOR) + " " + caseCOLSTR + ":" );
			}
			writeCode(node_iter->first, indentLevel + 2, latchingNodeId, ifFollowNodeId);
			if(codeLines->back().find("return") == std::string::npos)
				codeLines->push_back(getIndent(indentLevel + 2) + "break;");
		}
	}
	codeLines->push_back(getIndent(indentLevel) + "}");
}

void DecompiledCodeGenerator::writeTwoWay(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId){
	msg("writeTwoWay(%d, %d, %d, %d)\n", basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	writeBasicBlock(basicBlockId, indentLevel);
	
	int followNodeId = graphStructeringInfo->conditionals[basicBlockId].followNodeId;
	TwoWayNodePtr headerNode = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(basicBlockId));
	ExpressionPtr ifCondition = std::dynamic_pointer_cast<ConditionalJump>(headerNode->instructions->back())->condition;
	
	NodePtr trueSuccessor = controlFlowGraph->nodes->at(headerNode->trueSuccessorID);
	NodePtr falseSuccessor = controlFlowGraph->nodes->at(headerNode->falseSuccessorID);

	if(isAbnormalEdgeToLoopFollowNode(headerNode->id, headerNode->trueSuccessorID)){
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR))+ "(" + ifCondition->getExpressionCOLSTR() + ")");
		codeLines->push_back(getIndent(indentLevel + 1) + "break;");
		writeCode(headerNode->falseSuccessorID, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	else if(isAbnormalEdgeToLoopFollowNode(headerNode->id, headerNode->falseSuccessorID)){
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR))+ "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ")");
		codeLines->push_back(getIndent(indentLevel + 1) + "break;");
		writeCode(headerNode->trueSuccessorID, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	//else if(trueSuccessor->nodeType == Exit_NODE || falseSuccessor->nodeType == Exit_NODE){
	//	ExpressionPtr newIfCondition = (trueSuccessor->nodeType == Exit_NODE) ? ifCondition : getNegatedExpression(ifCondition);
	//	codeLines->push_back(getIndent(indentLevel)
	//		+ std::string(COLSTR("if", CONDITIONAL_COLOR))
	//		+ "(" + newIfCondition->getExpressionCOLSTR() + ") {");
	//	if(trueSuccessor->nodeType == Exit_NODE){	
	//		writeBasicBlock(trueSuccessor->id, indentLevel + 1/*, latchingNodeId, ifFollowNodeId*/);
	//		codeLines->push_back(getIndent(indentLevel) + "}");
	//		writeCode(falseSuccessor->id, indentLevel, latchingNodeId, ifFollowNodeId);
	//	}
	//	else{
	//		writeBasicBlock(falseSuccessor->id, indentLevel + 1/*, latchingNodeId, ifFollowNodeId*/);
	//		codeLines->push_back(getIndent(indentLevel) + "}");
	//		writeCode(trueSuccessor->id, indentLevel, latchingNodeId, ifFollowNodeId);
	//	}
	//}
	/*
	else if(hasSimplePathToExitNode(trueSuccessor->id)){ //TODO check if both branches have a simple path to exit node
		codeLines->push_back(getIndent(indentLevel)	+ std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + ifCondition->getExpressionCOLSTR() + ") {");
		writeSimplePathToExitNode(trueSuccessor->id, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel)	+ "}");
		writeCode(falseSuccessor->id, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	else if(hasSimplePathToExitNode(falseSuccessor->id)){
		codeLines->push_back(getIndent(indentLevel)	+ std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ") {");
		writeSimplePathToExitNode(falseSuccessor->id, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel)	+ "}");
		writeCode(trueSuccessor->id, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	*/
	else if(hasSimplePathToFollowkNode(trueSuccessor->id)){
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR))+ "(" + ifCondition->getExpressionCOLSTR() + ") {");
		writeSimplePathToFollowNode(trueSuccessor->id, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel) + "}");
		writeCode(falseSuccessor->id, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	else if(hasSimplePathToFollowkNode(falseSuccessor->id)){
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR))+ "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ") {");
		writeSimplePathToFollowNode(falseSuccessor->id, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel) + "}");
		writeCode(trueSuccessor->id, indentLevel, latchingNodeId, ifFollowNodeId);
	}

	//else if(trueSuccessor->id == latchingNodeId){//TODO check Lab
	//	codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR))+ "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ") {");
	//	writeCode(falseSuccessor->id, indentLevel + 1, latchingNodeId, ifFollowNodeId);
	//	codeLines->push_back(getIndent(indentLevel) + "}");
	//}
	//else if(falseSuccessor->id == latchingNodeId){
	//	codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR))+ "(" + ifCondition->getExpressionCOLSTR() + ") {");
	//	writeCode(trueSuccessor->id, indentLevel + 1, latchingNodeId, ifFollowNodeId);
	//	codeLines->push_back(getIndent(indentLevel) + "}");
	//}
	else if(followNodeId != -1){
		bool emptyThen = false;
		if(headerNode->trueSuccessorID != followNodeId){
			codeLines->push_back(getIndent(indentLevel)
				+ std::string(COLSTR("if", CONDITIONAL_COLOR))
				+ "(" + ifCondition->getExpressionCOLSTR() + ") {");
			if(handledNodeIds.find(headerNode->trueSuccessorID) == handledNodeIds.end() && !isAbnormalEdgeToLoopFollowNode(headerNode->id, headerNode->trueSuccessorID))
				writeCode(headerNode->trueSuccessorID, indentLevel + 1, latchingNodeId, followNodeId);
			else
				emitGotoLabel(headerNode->trueSuccessorID, indentLevel + 1);
		}
		else{
			codeLines->push_back(getIndent(indentLevel)
				+ std::string(COLSTR("if", CONDITIONAL_COLOR))
				+ "(" + getNegatedExpression(ifCondition)->getExpressionCOLSTR() + ") {");

			///if(handledNodeIds.find(headerNode->falseSuccessorID) == handledNodeIds.end() && !isAbnormalEdgeToLoopFollowNode(headerNode->id, headerNode->falseSuccessorID))
			if(handledNodeIds.find(headerNode->falseSuccessorID) == handledNodeIds.end())
				if(isAbnormalEdgeToLoopFollowNode(headerNode->id, headerNode->falseSuccessorID))
					codeLines->push_back(getIndent(indentLevel) + "break;");
				else
					writeCode(headerNode->falseSuccessorID, indentLevel + 1, latchingNodeId, followNodeId);
			else
				emitGotoLabel(headerNode->falseSuccessorID, indentLevel + 1);
			emptyThen = true;
		}

		/***************************/

		if(handledNodeIds.find(headerNode->falseSuccessorID) == handledNodeIds.end()){
			if(headerNode->falseSuccessorID != followNodeId){
				codeLines->push_back(getIndent(indentLevel) + "}");
				codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("else", CONDITIONAL_COLOR)) + "{");
				if(isAbnormalEdgeToLoopFollowNode(headerNode->id, headerNode->falseSuccessorID))
					emitGotoLabel(headerNode->falseSuccessorID, indentLevel + 1);
				else
					writeCode(headerNode->falseSuccessorID, indentLevel + 1, latchingNodeId, followNodeId);
			}
		}
		else if(!emptyThen){
			codeLines->push_back(getIndent(indentLevel) + "}");
			codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("else", CONDITIONAL_COLOR)) + "{");
			emitGotoLabel(headerNode->falseSuccessorID, indentLevel + 1);
		}

		codeLines->push_back(getIndent(indentLevel) + "}");
		if(followNodeId != latchingNodeId &&handledNodeIds.find(followNodeId) == handledNodeIds.end())
			writeCode(followNodeId, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	else{
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("if", CONDITIONAL_COLOR)) + "(" + ifCondition->getExpressionCOLSTR() + ") {");
		if(handledNodeIds.find(headerNode->trueSuccessorID) == handledNodeIds.end())
			writeCode(headerNode->trueSuccessorID, indentLevel + 1, latchingNodeId, ifFollowNodeId);
		else
			emitGotoLabel(headerNode->trueSuccessorID, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel) + "}");
		codeLines->push_back(getIndent(indentLevel) + std::string(COLSTR("else", CONDITIONAL_COLOR)) + "{");
		if(handledNodeIds.find(headerNode->falseSuccessorID) == handledNodeIds.end())
			writeCode(headerNode->falseSuccessorID, indentLevel + 1, latchingNodeId, ifFollowNodeId);
		else
			emitGotoLabel(headerNode->falseSuccessorID, indentLevel + 1);
		codeLines->push_back(getIndent(indentLevel) + "}");
	}
}

void DecompiledCodeGenerator::writeOneWay(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId){
	msg("writeOneWay(%d, %d, %d, %d)\n", basicBlockId, indentLevel, latchingNodeId, ifFollowNodeId);
	writeBasicBlock(basicBlockId, indentLevel);
	NodePtr currentNode = controlFlowGraph->nodes->at(basicBlockId);
	if(currentNode->nodeType == Exit_NODE/*RETURN_NODE*/)
		return;
	OneWayNodePtr currentOneWayNode = std::dynamic_pointer_cast<OneWayNode>(currentNode);
	///if(handledNodeIds.find(currentOneWayNode->successorID) == handledNodeIds.end() && !isAbnormalEdgeToLoopFollowNode(currentOneWayNode->id, currentOneWayNode->successorID))
	if(handledNodeIds.find(currentOneWayNode->successorID) == handledNodeIds.end()){
		if(isAbnormalEdgeToLoopFollowNode(currentOneWayNode->id, currentOneWayNode->successorID))
			codeLines->push_back(getIndent(indentLevel) + "break;");
		else
			writeCode(currentOneWayNode->successorID, indentLevel, latchingNodeId, ifFollowNodeId);
	}
	else
		emitGotoLabel(currentOneWayNode->successorID, indentLevel);
}

void DecompiledCodeGenerator::writeBasicBlock(int basicBlockId, int indentLevel){
	msg("writeBasicBlock(%d, %d)\n", basicBlockId, indentLevel);
	InstructionVectorPtr instructions = controlFlowGraph->nodes->at(basicBlockId)->instructions;
	for(std::vector<InstructionPtr>::iterator inst_iter = instructions->begin() ; inst_iter != instructions->end() ; ++inst_iter){
		InstructionPtr currentInstruction = *inst_iter;
		if(isNotJumpInstruction(currentInstruction)){
			codeLines->push_back(getIndent(indentLevel) + currentInstruction->getInstructionCOLSTR() + ";");
		}
	}
}

void DecompiledCodeGenerator::emitGotoLabel(int targetNode, int indentLevel){
	msg("emitGotoLabel(%d, %d)\n", targetNode, indentLevel);
	NodeType nodeType = controlFlowGraph->nodes->at(targetNode)->nodeType;
	/*if(nodeType == Exit_NODE || nodeType == RETURN_NODE)
		writeBasicBlock(targetNode, indentLevel);*/
	if(hasSimplePathToExitNode(targetNode)){
		writeSimplePathToExitNode(targetNode, indentLevel);
	}
	else{
		std::string label = "NODE_" + boost::lexical_cast<std::string>(targetNode);
		codeLines->push_back(getIndent(indentLevel) + "goto " + getCOLSTR(label, LABEL_COLOR));
		numberOfGotos++;
		usedLabels.insert(targetNode);
	}
}

bool DecompiledCodeGenerator::isSwitchHeader(int basicBlockId){
	return graphStructeringInfo->switches.find(basicBlockId) != graphStructeringInfo->switches.end();
}

bool DecompiledCodeGenerator::isLoopHeader(int basicBlockId){
	return graphStructeringInfo->loops.find(basicBlockId) != graphStructeringInfo->loops.end();
}

bool DecompiledCodeGenerator::isConditionalHeader(int basicBlockId){
	return graphStructeringInfo->conditionals.find(basicBlockId) != graphStructeringInfo->conditionals.end();
}

bool DecompiledCodeGenerator::isBreakNode(int basicBlockId){
	if(currentLoopHeaderId == -1){
		intSetPtr breakNodes = graphStructeringInfo->loops[currentLoopHeaderId].breakNodes;
		return breakNodes->find(basicBlockId) != breakNodes->end();
	}
	return false;
}

bool DecompiledCodeGenerator::isGotoNode(int basicBlockId){
	if(currentLoopHeaderId == -1){
		goToMapPtr gotoNodes = graphStructeringInfo->loops[currentLoopHeaderId].gotoNodeMap;
		return gotoNodes->find(basicBlockId) != gotoNodes->end();
	}
	return false;
}

bool DecompiledCodeGenerator::isAbnormalEdgeToLoopFollowNode(int source, int target){
	bool result = false;
	if(currentLoopHeaderId != -1){
		intSetPtr loopNodes = graphStructeringInfo->loops[currentLoopHeaderId].loopNodes;
		//int followNodeId = graphStructeringInfo->loops[currentLoopHeaderId].followNodeId;
		if(loopNodes->find(source) != loopNodes->end()){
			result = (target == graphStructeringInfo->loops[currentLoopHeaderId].followNodeId);
			/*IndexMap index = boost::get(boost::vertex_index, graph);
			Vertex node = boost::vertex(basicBlockId, graph);
			OutEdgeIterator e, end;
			for(boost::tie(e, end) = boost::out_edges(node, graph) ; e != end ; ++e){
				if(index[boost::target(*e, graph)] == followNodeId)
					return true;
			}*/
		}
	}
	//msg("isAbnormalEdgeToLoopFollowNode(%d, %d) = %d\n", source, target, result);
	//msg("\tcurrentLoopHeaderId = %d\n", currentLoopHeaderId);
	return result;
}

bool DecompiledCodeGenerator::isNotJumpInstruction(const InstructionPtr &instruction){
	return instruction->type != CONDITIONAL_JUMP
		&& instruction->type != DIRECT_JUMP
		&& instruction->type != INDIRECT_JUMP
		&& instruction->type != INDIRECT_TABLE_JUMP;
}

std::string DecompiledCodeGenerator::getIndent(int indentLevel){
	return std::string(indentLevel * 4, ' ');
}

bool DecompiledCodeGenerator::isDirectSuccessor(int node, int head){
	intSetPtr headSuccessors = controlFlowGraph->nodes->at(head)->getSuccessors();
	return headSuccessors->find(node) != headSuccessors->end();
}

bool DecompiledCodeGenerator::isNodeHandled(int nodeId){
	return handledNodeIds.find(nodeId) != handledNodeIds.end();
}

bool DecompiledCodeGenerator::hasSimplePathToExitNode(int nodeId){
	std::set<int> pathNodes;
	return hasLoopFreeExit(nodeId, -1, pathNodes);
	//return false;
	/*NodePtr node = controlFlowGraph->nodes->at(nodeId);
	if(node->nodeType == Exit_NODE)
		return true;
	else if(node->nodeType == ONE_WAY_NODE){
		OneWayNodePtr oneWayNode = std::dynamic_pointer_cast<OneWayNode>(node);
		return hasSimplePathToExitNode(oneWayNode->successorID);
	}
	else{
		return false;
	}*/
}

void DecompiledCodeGenerator::writeSimplePathToExitNode(int nodeId, int indentLevel){
	writeBasicBlock(nodeId, indentLevel);
	NodePtr node = controlFlowGraph->nodes->at(nodeId);
	if(node->nodeType == ONE_WAY_NODE){
		int successorId = std::dynamic_pointer_cast<OneWayNode>(node)->successorID;
		writeSimplePathToExitNode(successorId, indentLevel);
	}
}

bool DecompiledCodeGenerator::hasSimplePathToFollowkNode(int nodeId){
	if(currentLoopHeaderId != -1){
		std::set<int> pathNodes;
		return hasLoopFreeExit(nodeId, graphStructeringInfo->loops[currentLoopHeaderId].followNodeId, pathNodes);
		/*NodePtr node = controlFlowGraph->nodes->at(nodeId);
		if(nodeId == graphStructeringInfo->loops[currentLoopHeaderId].followNodeId){
			return true;
		}
		else if(node->nodeType == ONE_WAY_NODE){
			int successorId = std::dynamic_pointer_cast<OneWayNode>(node)->successorID;
			return hasSimplePathToFollowkNode(successorId);
		}*/
	}
	return false;
}

void DecompiledCodeGenerator::writeSimplePathToFollowNode(int nodeId, int indentLevel){
	int loopFollowNodeId = graphStructeringInfo->loops[currentLoopHeaderId].followNodeId;
	int currentNodeId = nodeId;
	while(currentNodeId != loopFollowNodeId){
		writeBasicBlock(currentNodeId, indentLevel);
		currentNodeId = std::dynamic_pointer_cast<OneWayNode>(controlFlowGraph->nodes->at(currentNodeId))->successorID;
	}
	codeLines->push_back( getIndent(indentLevel) + "break;");
}


bool DecompiledCodeGenerator::hasLoopFreeExit(int nodeId, int exitNodeId, std::set<int>& pathNodes){
	if(pathNodes.find(nodeId) == pathNodes.end()){
		pathNodes.insert(nodeId);
		NodePtr node = controlFlowGraph->nodes->at(nodeId);
		if(node->nodeType == Exit_NODE || nodeId == exitNodeId)
			return true;
		else if(node->nodeType == ONE_WAY_NODE){
			int successorId = std::dynamic_pointer_cast<OneWayNode>(node)->successorID;
			return hasLoopFreeExit(successorId, exitNodeId, pathNodes);
		}
	}
	return false;
}
