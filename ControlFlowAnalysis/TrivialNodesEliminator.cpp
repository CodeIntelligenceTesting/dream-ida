////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TrivialNodesEliminator.h"
#include "../IntermediateRepresentation/ControlFlowGraph.h"

#include <gdl.hpp>

TrivialNodesEliminator::TrivialNodesEliminator(ControlFlowGraphPtr _controlFlowGraph)
{
	controlFlowGraph = _controlFlowGraph;
}

TrivialNodesEliminator::~TrivialNodesEliminator(void)
{
}

void TrivialNodesEliminator::eliminateTrivialNodes(){
	removeDirectJumpInstructions();
	OneWayNodePtr emptyOneWayNode = getAnEmptyOneWayNode();
	while(emptyOneWayNode.get() != NULL){
		updateSuccessorsOfPredecessorNodes(emptyOneWayNode);
		controlFlowGraph->removeNode(emptyOneWayNode->id);
		emptyOneWayNode = getAnEmptyOneWayNode();
	}
	/*std::set<int> emptyNodes;
	getEmptyNodes(controlFlowGraph, emptyNodes);
	for(std::set<int>::iterator id = emptyNodes.begin() ; id != emptyNodes.end() ; ++id){
		NodePtr emptyNode = controlFlowGraph->nodes->at(*id);
		if(emptyNode->nodeType == ONE_WAY_NODE){
			msg("Supposed to be empty(%d instructions)\n", emptyNode->instructions->size());
			OneWayNodePtr emptyOneWayNode = std::dynamic_pointer_cast<OneWayNode>(emptyNode);
			updateSuccessorsOfPredecessorNodes(controlFlowGraph, emptyOneWayNode);
			removeNode(controlFlowGraph, emptyOneWayNode->id);
		}
		else{
			msg("Error: An empty node of type %d\n", emptyNode->nodeType);
		}
	}*/
}

OneWayNodePtr TrivialNodesEliminator::getAnEmptyOneWayNode(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr node = node_iter->second;
		if(node->instructions->empty() && node->nodeType == ONE_WAY_NODE && node->id != 0)
			return std::dynamic_pointer_cast<OneWayNode>(node);
	}
	return std::shared_ptr<OneWayNode>();
}

void TrivialNodesEliminator::removeDirectJumpInstructions(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr node = node_iter->second;
		if(!node->instructions->empty() && node->instructions->back()->type == DIRECT_JUMP){
			node->instructions->pop_back();
		}
	}
}

void TrivialNodesEliminator::updateSuccessorsOfPredecessorNodes(const OneWayNodePtr &emptyOneWayNode){
	intSetPtr predecessors = controlFlowGraph->getPredecessors(emptyOneWayNode->id);
	for(std::set<int>::iterator predId = predecessors->begin() ; predId != predecessors->end() ; ++predId){
		NodePtr predNode = controlFlowGraph->nodes->at(*predId);
		if(predNode->nodeType == TWO_WAY_NODE){
			TwoWayNodePtr predTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(predNode);
			if(predTwoWayNode->falseSuccessorID == emptyOneWayNode->id)
				predTwoWayNode->falseSuccessorID = emptyOneWayNode->successorID;
			else if(predTwoWayNode->trueSuccessorID == emptyOneWayNode->id)
				predTwoWayNode->trueSuccessorID = emptyOneWayNode->successorID;
		}
		else if(predNode->nodeType == ONE_WAY_NODE){
			OneWayNodePtr predOneWayNode = std::dynamic_pointer_cast<OneWayNode>(predNode);
			predOneWayNode->successorID = emptyOneWayNode->successorID;
		}
		else if(predNode->nodeType == N_WAY_NODE){

			int emptyOneWayNodeId = emptyOneWayNode->id;
			int emptyOneWayNodeSuccessorId = emptyOneWayNode->successorID;

			intSetPtr newSuccessors = std::make_shared<std::set<int>>();
			N_WayNodePtr nWayPredNode = std::dynamic_pointer_cast<N_WayNode>(predNode);
			for(std::set<int>::iterator succ_iter = nWayPredNode->successorIDs->begin() ; succ_iter != nWayPredNode->successorIDs->end() ; ++succ_iter)
				newSuccessors->insert((*succ_iter != emptyOneWayNodeId) ? emptyOneWayNodeId : emptyOneWayNodeSuccessorId);	
			nWayPredNode->successorIDs = newSuccessors;

			InstructionPtr lastInstruction = nWayPredNode->instructions->back();
			if(lastInstruction->type == INDIRECT_TABLE_JUMP){
				IndirectTableJumpPtr switchInst = std::dynamic_pointer_cast<IndirectTableJump>(lastInstruction);
				if(switchInst->defaultBlockId == emptyOneWayNodeId)
					switchInst->defaultBlockId = emptyOneWayNodeSuccessorId;

				nodeToCasesMapPtr newNodeToCasesMap = std::make_shared<std::map<int, std::set<int>>>();
				for(std::map<int, std::set<int>>::iterator n_it = switchInst->nodeToCasesMap->begin() ; n_it != switchInst->nodeToCasesMap->end() ; ++n_it){
					int newNodeId = (n_it->first != emptyOneWayNodeId) ? emptyOneWayNodeId : emptyOneWayNodeSuccessorId;
					newNodeToCasesMap->operator[](newNodeId) = n_it->second;
				}
				switchInst->nodeToCasesMap = newNodeToCasesMap;
			}
		}
	}
}

//bool TrivialNodesEliminator::hasOnlyDirectJumpInstruction(NodePtr node){
//	return node->instructions->size() == 1 && node->instructions->front()->type == DIRECT_JUMP;
//}


//
//void EmptyNodesEliminator::getEmptyNodes(const ControlFlowGraphPtr &controlFlowGraph, std::set<int> &emptyNodes_out){
//	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
//		NodePtr currentNode = node_iter->second;
//		if(currentNode->instructions->empty()){
//			emptyNodes_out.insert(currentNode->id);
//		}
//	}
//}
//
//
//void EmptyNodesEliminator::removeNode(ControlFlowGraphPtr &controlFlowGraph, int nodeToRemoveId){
//	intToNodeMapPtr nodes = controlFlowGraph->nodes;
//	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
//		updateNodeEdges(node_iter->second, nodeToRemoveId);
//		if(node_iter->second->id > nodeToRemoveId){
//			int previousID = node_iter->first - 1;
//			nodes->operator[](previousID) = node_iter->second;
//			nodes->operator[](previousID)->id = previousID;
//			//std::swap(nodes->at(node_iter->first - 1), node_iter->second);
//		}
//	}
//	nodes->erase(std::prev(nodes->end()));
//}
//
//void EmptyNodesEliminator::updateNodeEdges(NodePtr &node, int removedNodeId){
//	if(node->nodeType == ONE_WAY_NODE){
//		OneWayNodePtr oneWayNode = std::dynamic_pointer_cast<OneWayNode>(node);
//		if(oneWayNode->successorID > removedNodeId){
//			oneWayNode->successorID--;
//			updateLastInstructionIfBranch(node);
//		}
//	}
//	else if(node->nodeType == TWO_WAY_NODE){
//		TwoWayNodePtr twoWayNode = std::dynamic_pointer_cast<TwoWayNode>(node);
//		if(twoWayNode->trueSuccessorID > removedNodeId){
//			twoWayNode->trueSuccessorID--;
//			updateLastInstructionIfBranch(node);
//		}
//		if(twoWayNode->falseSuccessorID > removedNodeId)
//			twoWayNode->falseSuccessorID--;
//	}
//}
//
//void EmptyNodesEliminator::updateLastInstructionIfBranch(NodePtr &node){
//	InstructionVectorPtr instructions = node->instructions;
//	if(!instructions->empty()){
//		InstructionPtr lastInstruction = instructions->back();
//		if(lastInstruction->type == DIRECT_JUMP){
//			std::dynamic_pointer_cast<DirectJump>(lastInstruction)->targetBlockId = std::dynamic_pointer_cast<OneWayNode>(node)->successorID;
//		}
//		else if(lastInstruction->type == CONDITIONAL_JUMP){
//			std::dynamic_pointer_cast<ConditionalJump>(lastInstruction)->target = std::dynamic_pointer_cast<TwoWayNode>(node)->trueSuccessorID;
//		}
//	}
//}
