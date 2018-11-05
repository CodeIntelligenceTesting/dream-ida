/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "IntermediateRepresentation/IRSharedPointers.h"
#include "../ControlFlowAnalysis/GraphTypes.h"

#include "../TypeAnalysis/TypeSharedPointers.h"

class DecompiledCodeGenerator
{
public:
	DecompiledCodeGenerator(ControlFlowGraphPtr _controlFlowGraph,
							GraphStructeringInfoPtr _graphStructeringInfo,
							ExpressionVectorPtr _functionArguments,
							std::string _functionName);
	virtual ~DecompiledCodeGenerator(void);

	void generateCodeLines(TypeMapPtr typeMap);
	stringVectorPtr codeLines;
	std::map<int, int> lineNumberToLabelMap;
	std::set<int> usedLabels;

	int numberOfGotos;
private:
	ControlFlowGraphPtr controlFlowGraph;
	Graph graph;
	GraphStructeringInfoPtr graphStructeringInfo;
	ExpressionVectorPtr functionArguments;
	std::string functionName;
	std::set<int> handledNodeIds;
	int currentLoopHeaderId;
	
	void writeCode(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId);
	void writeLoop(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId);
	void writeNWay(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId);
	void writeTwoWay(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId);
	void writeOneWay(int basicBlockId, int indentLevel, int latchingNodeId, int ifFollowNodeId);
	void writeBasicBlock(int basicBlockId, int indentLevel);
	void emitGotoLabel(int targetNode, int indentLevel);

	bool isSwitchHeader(int basicBlockId);
	bool isLoopHeader(int basicBlockId);
	bool isConditionalHeader(int basicBlockId);
	bool isBreakNode(int basicBlockId);
	bool isGotoNode(int basicBlockId);
	bool isAbnormalEdgeToLoopFollowNode(int source, int target);
	bool isNotJumpInstruction(const InstructionPtr &instruction);
	bool isDirectSuccessor(int node, int head);
	bool isNodeHandled(int nodeId);
	

	bool hasSimplePathToExitNode(int nodeId);
	void writeSimplePathToExitNode(int nodeId, int indentLevel);

	bool hasSimplePathToFollowkNode(int nodeId);
	void writeSimplePathToFollowNode(int nodeId, int indentLevel);

	bool hasLoopFreeExit(int nodeId, int exitNodeId, std::set<int>& pathNodes);

	std::string getIndent(int indentLevel);
};

