/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <memory>
#include <utility>

#include <ida.hpp>
#include <idp.hpp>
#include <gdl.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "IRHeaders.h"
#include "../IntermediateRepresentation/IRSharedPointers.h"

class ControlFlowGraph
{
public:
	ControlFlowGraph(std::string _functionName/*, flowChartPtr _flow_chart*/);
	virtual ~ControlFlowGraph(void);

	void addNode(int nodeId, InstructionVectorPtr inst_list, qbasic_block_t nodeBlock);
	void doneAddingNodes(); void updateGraph();
	intSetPtr getSuccessors(int nodeId);
	intSetPtr getPredecessors(int nodeId);
	bool pathExists(int srcId, int dstId);
	bool doesExistCFGPath(const InstructionPtr& srcInst, const InstructionPtr&  dstInst);
	bool cfgPathExists(const InstructionPtr& srcInst, const InstructionPtr& dstInst, const InstructionPtr& passingByInst);

	void computeInstructionIds(const InstructionPtr& instruction, std::pair<int, int>& ids);

	void removeNode(int nodeToRemoveId);
	std::string to_json();
	//void splitReturnNodesWithMultipleIncomingEndges();

	intToNodeMapPtr nodes;
	std::string functionName;
private:
	//flowChartPtr flow_chart;
	int** nodeDistances;

	void getNodeDistances();
	void updateNodeEdges(NodePtr &node, int removedNodeId);
	void updateLastInstructionIfBranch(NodePtr &node);

	void freeDistanceMatrix();
	void resetNodesPredecessors();
	void updateNodesPredecessors();

	void splitExitNodesWithMultipePredecessors();
	ExitNodePtr getExitNodeWithMultipePredecessors();
	ExitNodePtr getExitNodeWithOneWayPredecessors();
	void copyNodeCode(NodePtr sourceNode, NodePtr targetNode);
	void mergeIntoExitNode(NodePtr sourceNode, NodePtr targetNode);
	void updateContiningNode(NodePtr node);
};
