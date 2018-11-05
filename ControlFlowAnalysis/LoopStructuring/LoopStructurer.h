/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../DerivedGraphSequence/DerivedSequenceItem.h"
#include "../../IntermediateRepresentation/IRSharedPointers.h"
#include "LoopTypes.h"
#include "../GraphStructeringInfo.h"

struct Loop;

struct LoopTestedNodes{
	LoopTestedNodes(Vertex _testedNode, Vertex _latchingNode, Vertex _header){
		testedNode = _testedNode;
		latchingNode = _latchingNode;
		header = _header;
	};

	Vertex testedNode;
	Vertex latchingNode;
	Vertex header;
};

class LoopStructurer
{
public:
	LoopStructurer(ControlFlowGraphPtr _controlFlowGraph);
	~LoopStructurer(void);

	void structureLoops(GraphStructeringInfoPtr &graphStructeringInfo);
private:
	ControlFlowGraphPtr controlFlowGraph;
	DerivedGraphSequence derivedGraphSequence;

	bool getBackEdgeNode(Vertex header, DerivedSequenceItem &derivedSequenceItem, Vertex &latchingNode);

	intSetPtr getLoopNodes(int level, Vertex header, Vertex latchingNode, std::set<Vertex> &intervalNodes);
	bool getOriginalHeaderLatchingNodes(std::set<Vertex> &headerOrigNodes, std::set<Vertex> &latchingOrigNodes, std::pair<Vertex, Vertex> &headerLatchingPair);
	bool doesEdgeExist(Vertex start, Vertex end, const Graph &g);
	//intVectorPtr getReversePostorder(const Graph &g);
	//int getReversePostorderNumber(int nodeIndex, intVectorPtr &reversePostorder);
	bool doesExistPathInIntervalContainingLatchingNode(Graph graph, const LoopTestedNodes &loopTestedNodes, std::set<Vertex> &intervalNodes);
	void removeIntervalEdgesWhoseTargetsNotInInterval(Graph &graph, std::set<Vertex> &intervalNodes);
	bool getPathVertices(const Graph &graph, Vertex start, Vertex end, std::set<Vertex> &pathVertices);

	LoopType getLoopType(Vertex header, Vertex latchingNode, const intSetPtr &loopNodes);
	LoopType getOneNodeLoopType(Vertex loopNode, const intSetPtr &loopNodes);
	bool hasSuccessorNotInLoopNodes(Vertex testedNode, const intSetPtr &loopNodes, const Graph &graph);

	void findFollowNodeId(Loop &loop_out);
	void findFollowNodeOfPreTestedLoop(Loop &loop_out);
	void findFollowNodeOfPostTestedLoop(Loop &loop_out);
	void findFollowNodeOfEndlessLoop(Loop &loop_out);
	int findSuccessorNotInSet(int sourceNodeId, const intSetPtr &loopNodes);

	void findExitNodes(Loop &loop_out);
	bool haveToCheckNode(int nodeId, const Loop &containingLoop);
};
