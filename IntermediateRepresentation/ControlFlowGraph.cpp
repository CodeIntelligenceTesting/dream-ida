////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ControlFlowGraph.h"
#include "../Shared/houseKeepingUtilities.h"
#include <boost/lexical_cast.hpp>


ControlFlowGraph::ControlFlowGraph(std::string _functionName)
{
	nodes = std::make_shared<std::map<int, NodePtr>>();
	functionName = _functionName;
	nodeDistances = NULL;
}


ControlFlowGraph::~ControlFlowGraph(void)
{
	freeDistanceMatrix();
	//deleteMapToPointers(nodes); nodes = NULL;
	//delete flow_chart; flow_chart = NULL;
}


void ControlFlowGraph::addNode(int nodeId, InstructionVectorPtr inst_list, qbasic_block_t nodeBlock){
	//qbasic_block_t currentBlock = flow_chart->blocks[nodeId];
	for(std::vector<InstructionPtr>::iterator inst_iter = inst_list->begin() ; inst_iter != inst_list->end() ; ++inst_iter){
		(*inst_iter)->ContainingNodeId = nodeId;
	}
	switch(nodeBlock.succ.size()){
	case 0:
		(*nodes)[nodeId] = std::make_shared<ExitNode>(nodeId, nodeBlock.startEA, nodeBlock.endEA, inst_list);
		break;
	case 1:
		(*nodes)[nodeId] = std::make_shared<OneWayNode>(nodeId, nodeBlock.startEA, nodeBlock.endEA, inst_list, nodeBlock.succ[0]);
		break;
	case 2:
		{
			int trueSuccessorID = nodeBlock.succ[0];
			int falseSuccessorID = nodeBlock.succ[1];
			if(inst_list->back()->type == CONDITIONAL_JUMP){
				ConditionalJumpPtr cond_inst = std::dynamic_pointer_cast<ConditionalJump>(inst_list->back());
				if(cond_inst->target != trueSuccessorID){
					int tmp = trueSuccessorID;
					trueSuccessorID = falseSuccessorID;
					falseSuccessorID = tmp;
				}
				(*nodes)[nodeId] = std::make_shared<TwoWayNode>(nodeId, nodeBlock.startEA, nodeBlock.endEA, inst_list, trueSuccessorID, falseSuccessorID);
			}
			else{
				//msg("%x\n", inst_list->back()->address);
				msg("Two-way switch node\n");
				//(*nodes)[nodeId] = NULL;
				intSetPtr successorIDs = std::make_shared<std::set<int>>();
				for(qvector<int>::iterator iter = nodeBlock.succ.begin() ; iter != nodeBlock.succ.end() ; iter++){
					successorIDs->insert(*iter);
				}
				(*nodes)[nodeId] = std::make_shared<N_WayNode>(nodeId, nodeBlock.startEA, nodeBlock.endEA, inst_list, successorIDs);
			}
		}
		break;
	default:
		{
			intSetPtr successorIDs = std::make_shared<std::set<int>>();
			for(qvector<int>::iterator iter = nodeBlock.succ.begin() ; iter != nodeBlock.succ.end() ; iter++){
				successorIDs->insert(*iter);
			}
			(*nodes)[nodeId] = std::make_shared<N_WayNode>(nodeId, nodeBlock.startEA, nodeBlock.endEA, inst_list, successorIDs);
		}
	}
}

void ControlFlowGraph::doneAddingNodes(){
	getNodeDistances();
	updateNodesPredecessors();
	//splitExitNodesWithMultipePredecessors();
}

void ControlFlowGraph::updateGraph(){
	freeDistanceMatrix();
	getNodeDistances();
	updateNodesPredecessors();
}

void ControlFlowGraph::getNodeDistances(){
	typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_weight_t, int>> Graph_t;
	typedef boost::graph_traits<Graph_t>::vertex_descriptor Vertex_t;
	typedef boost::property_map<Graph_t, boost::vertex_index_t>::type IndexMap;

	typedef std::pair<int, int> Edge_t;
	
	int numOfVertices = nodes->size();
	nodeDistances = new int*[numOfVertices];
	for(int i = 0 ; i < numOfVertices ; i++){
		nodeDistances[i] = new int[numOfVertices];
		for(int j = 0 ; j < numOfVertices ; j++){
			nodeDistances[i][j] = -1;
		}
	}
	
	std::vector<Edge_t> graphEdges;
	int num_edges = 0;
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
		int nodeId = node_iter->first;
		//intseq_t successors = flow_chart->blocks[nodeId].succ;
		intSetPtr successors = node_iter->second->getSuccessors();
		for(std::set<int>::iterator succ_iter = successors->begin() ; succ_iter != successors->end() ; ++succ_iter){
			graphEdges.push_back(Edge_t(nodeId, *succ_iter));
			num_edges++;
		}
	}

	if(num_edges == 0)
		return;
	/*int * weights = new int[num_edges];
	for(int i = 0 ; i < num_edges ; i++){
		weights[i] = 1;
	}*/
	std::vector<int> weightsVector(num_edges, 1);
	int* weights = &weightsVector[0];

	Graph_t cfg(graphEdges.begin(), graphEdges.end(), weights, numOfVertices);

	std::vector<Vertex_t> p(numOfVertices);
	std::vector<int> d(numOfVertices);
	boost::graph_traits<Graph_t>::vertex_iterator iter1, iterEnd1, iter2, iterEnd2;
	for(boost::tie(iter1, iterEnd1) = boost::vertices(cfg) ; iter1 != iterEnd1 ; ++iter1){
		boost::dijkstra_shortest_paths(cfg, *iter1, boost::predecessor_map(&p[0]).distance_map(&d[0]));
		for(boost::tie(iter2, iterEnd2) = boost::vertices(cfg) ; iter2 != iterEnd2 ; ++iter2){
			nodeDistances[*iter1][*iter2] = (d[*iter2] != (std::numeric_limits<int>::max)()) ? d[*iter2] : -1;
		}
	}
}

intSetPtr ControlFlowGraph::getSuccessors(int nodeId){
	return nodes->at(nodeId)->getSuccessors(); //flow_chart->blocks[nodeId].succ;
}

intSetPtr ControlFlowGraph::getPredecessors(int nodeId){
	return nodes->at(nodeId)->getPredecessors();//flow_chart->blocks[nodeId].pred;
}

bool ControlFlowGraph::doesExistCFGPath(const InstructionPtr& srcInst, const InstructionPtr&  dstInst){
	int srcNodeId = srcInst->ContainingNodeId;
	int dstNodeId = dstInst->ContainingNodeId;
	
	if(srcNodeId == dstNodeId){
		InstructionVectorPtr instructions = (*nodes)[srcNodeId]->instructions;
		std::vector<InstructionPtr>::iterator first = instructions->begin(), second = instructions->end();
		int srcInstId = std::distance(first, std::find(first, second, srcInst));

		instructions = (*nodes)[dstNodeId]->instructions;
		first = instructions->begin();
		second = instructions->end();
		int dstInstId = std::distance(first, std::find(first, second, dstInst));
		
		if(srcInstId < dstInstId){
			return true;
		}
		else{
			intSetPtr srcNodeSuccs = getSuccessors(srcNodeId);
			for(std::set<int>::iterator succ_iter = srcNodeSuccs->begin() ; succ_iter != srcNodeSuccs->end() ; ++succ_iter){
				if(pathExists(*succ_iter, dstNodeId))
					return true;
			}
			return false;
		}
	}
	else{
		return pathExists(srcNodeId, dstNodeId);
	}
}

bool ControlFlowGraph::pathExists(int srcId, int dstId){
	return nodeDistances[srcId][dstId] != -1;
}

bool ControlFlowGraph::cfgPathExists(const InstructionPtr& srcInst, const InstructionPtr& dstInst, const InstructionPtr& passingByInst){
	//return doesExistCFGPath(srcInst, passingByInst) && doesExistCFGPath(passingByInst, dstInst);
	int srcNodeId = srcInst->ContainingNodeId;
	int dstNodeId = dstInst->ContainingNodeId;
	int passingByNodeId = passingByInst->ContainingNodeId;

	std::vector<InstructionPtr>::iterator first = nodes->operator[](srcNodeId)->instructions->begin();
	std::vector<InstructionPtr>::iterator second = nodes->operator[](srcNodeId)->instructions->end();
	int srcInstId = std::distance(first, std::find(first, second, srcInst));

	first = nodes->operator[](dstNodeId)->instructions->begin();
	second = nodes->operator[](dstNodeId)->instructions->end();
	int dstInstId = std::distance(first, std::find(first, second, dstInst));

	first = nodes->operator[](passingByNodeId)->instructions->begin();
	second = nodes->operator[](passingByNodeId)->instructions->end();
	int passingByInstId = std::distance(first, std::find(first, second, passingByInst));

	if(srcNodeId == dstNodeId){
		if(srcInstId < dstInstId){
			if(passingByNodeId == srcNodeId){
				return passingByInstId > srcInstId && passingByInstId < dstInstId;
			}
			else
				return false;
		}
		else{
			//check for reverse path
		}
	}
	else if(pathExists(srcNodeId, passingByNodeId) && pathExists(passingByNodeId, dstNodeId)){
		if(srcNodeId == passingByNodeId){
			if(srcInstId < passingByInstId){
				return true;
			}
		}
		else if(dstNodeId == passingByNodeId){
			if(dstInstId > passingByInstId){
				return true;
			}
		}
		else{
			return true;
		}
	}
	return pathExists(srcNodeId, passingByNodeId) && pathExists(passingByNodeId, dstNodeId);
	//return false;
}

void ControlFlowGraph::computeInstructionIds(const InstructionPtr& instruction, std::pair<int, int>& ids){
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
		InstructionVectorPtr blockInstructions = node_iter->second->instructions;
		std::vector<InstructionPtr>::iterator inst_iter = std::find(blockInstructions->begin() , blockInstructions->end(), instruction);
		if(inst_iter != blockInstructions->end()){
			ids.first = node_iter->first;
			ids.second = std::distance(blockInstructions->begin(), inst_iter);
			return;
		}
	}
	ids.first = -1;
	ids.second = -1;
}

//void ControlFlowGraph::splitReturnNodesWithMultipleIncomingEndges(){
//	std::set<int> nodesToSplit;
//	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
//		if(node_iter->second->nodeType == RETURN_NODE && getPredecessors(node_iter->first)->size() > 2){
//			msg("Remove return node %d\n", node_iter->first);
//		}
//	}
//}

void ControlFlowGraph::removeNode(int nodeToRemoveId){
	for(unsigned int nodeId = 0 ; nodeId < nodes->size() ; ++nodeId){
		NodePtr currentNode = nodes->at(nodeId);
		updateNodeEdges(currentNode, nodeToRemoveId);
		if(currentNode->id > nodeToRemoveId){
			currentNode->id--;
			nodes->operator[](currentNode->id) = currentNode;
			updateContiningNode(currentNode);
			//std::swap(nodes->at(node_iter->first - 1), node_iter->second);
		}
	}
	nodes->erase(std::prev(nodes->end()));
	
	//update distance matrix
	updateGraph();
}

void ControlFlowGraph::updateNodeEdges(NodePtr &node, int removedNodeId){
	if(node->nodeType == ONE_WAY_NODE){
		OneWayNodePtr oneWayNode = std::dynamic_pointer_cast<OneWayNode>(node);
		if(oneWayNode->successorID > removedNodeId){
			oneWayNode->successorID--;
			updateLastInstructionIfBranch(node);
		}
	}
	else if(node->nodeType == TWO_WAY_NODE){
		TwoWayNodePtr twoWayNode = std::dynamic_pointer_cast<TwoWayNode>(node);
		if(twoWayNode->trueSuccessorID > removedNodeId){
			twoWayNode->trueSuccessorID--;
			updateLastInstructionIfBranch(node);
		}
		if(twoWayNode->falseSuccessorID > removedNodeId)
			twoWayNode->falseSuccessorID--;
	}
	else if(node->nodeType == N_WAY_NODE){
		intSetPtr newSuccessors = std::make_shared<std::set<int>>();
		N_WayNodePtr nWayNode = std::dynamic_pointer_cast<N_WayNode>(node);
		for(std::set<int>::iterator succ_iter = nWayNode->successorIDs->begin() ; succ_iter != nWayNode->successorIDs->end() ; ++succ_iter)
			newSuccessors->insert((*succ_iter > removedNodeId) ? *succ_iter - 1 : *succ_iter);	
		nWayNode->successorIDs = newSuccessors;

		InstructionPtr lastInstruction = nWayNode->instructions->back();
		if(lastInstruction->type == INDIRECT_TABLE_JUMP){
			IndirectTableJumpPtr switchInst = std::dynamic_pointer_cast<IndirectTableJump>(lastInstruction);
			if(switchInst->defaultBlockId > removedNodeId)
				switchInst->defaultBlockId --;

			nodeToCasesMapPtr newNodeToCasesMap = std::make_shared<std::map<int, std::set<int>>>();
			for(std::map<int, std::set<int>>::iterator n_it = switchInst->nodeToCasesMap->begin() ; n_it != switchInst->nodeToCasesMap->end() ; ++n_it){
				int newNodeId = (n_it->first > removedNodeId) ? n_it->first - 1 : n_it->first;
				newNodeToCasesMap->operator[](newNodeId) = n_it->second;
			}
			switchInst->nodeToCasesMap = newNodeToCasesMap;
		}
	}
}

void ControlFlowGraph::updateLastInstructionIfBranch(NodePtr &node){
	InstructionVectorPtr instructions = node->instructions;
	if(!instructions->empty()){
		InstructionPtr lastInstruction = instructions->back();
		if(lastInstruction->type == DIRECT_JUMP){
			std::dynamic_pointer_cast<DirectJump>(lastInstruction)->targetBlockId = std::dynamic_pointer_cast<OneWayNode>(node)->successorID;
		}
		else if(lastInstruction->type == CONDITIONAL_JUMP){
			std::dynamic_pointer_cast<ConditionalJump>(lastInstruction)->target = std::dynamic_pointer_cast<TwoWayNode>(node)->trueSuccessorID;
		}
	}
}


void ControlFlowGraph::freeDistanceMatrix(){
	if(nodeDistances != NULL){
		for(unsigned int i = 0 ; i < nodes->size() ; i++){
			delete[] nodeDistances[i];
		}
		delete[] nodeDistances;
	}
	nodeDistances = NULL;
}

void ControlFlowGraph::resetNodesPredecessors(){
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
		node_iter->second->getPredecessors()->clear();
	}
}

void ControlFlowGraph::updateNodesPredecessors(){
	resetNodesPredecessors();
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
		intSetPtr successors = node_iter->second->getSuccessors();
		for(std::set<int>::iterator succ_iter = successors->begin() ; succ_iter != successors->end() ; ++succ_iter){
			nodes->at(*succ_iter)->getPredecessors()->insert(node_iter->second->id);
		}
	}
}


void ControlFlowGraph::splitExitNodesWithMultipePredecessors(){
	//return;
	ExitNodePtr exitNode = getExitNodeWithOneWayPredecessors();
	while(exitNode.get() != NULL){
		intSetPtr exitNodePredecessors = exitNode->getPredecessors();
		msg("exit node(%d) with %d preds\n", exitNode->id, exitNodePredecessors->size());
		bool removeExitNode = true;
		std::set<int> predecessorsToRemove;
		for(std::set<int>::iterator pred_iter = exitNodePredecessors->begin() ; pred_iter != exitNodePredecessors->end() ; ++pred_iter){
			NodePtr predecessorNode = nodes->at(*pred_iter);
			if(predecessorNode->nodeType == ONE_WAY_NODE){
				mergeIntoExitNode(predecessorNode, exitNode);
				predecessorsToRemove.insert(predecessorNode->id);
			}
			else
				removeExitNode = false;
		}
		if(removeExitNode)
			removeNode(exitNode->id);
		else
			for(std::set<int>::iterator pred_iter = predecessorsToRemove.begin() ; pred_iter != predecessorsToRemove.end() ; ++pred_iter)
				exitNode->removePredecessor(*pred_iter);
		exitNode = getExitNodeWithOneWayPredecessors();
	}
}

ExitNodePtr ControlFlowGraph::getExitNodeWithMultipePredecessors(){
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		msg("----------------------->>>>>>> node(%d): type %d, nPreds = %d\n", currentNode->id, currentNode->nodeType, currentNode->getPredecessors()->size());
		if(currentNode->nodeType == Exit_NODE && currentNode->getPredecessors()->size() >= 2){
			msg("returning\n");
			return std::dynamic_pointer_cast<ExitNode>(currentNode);
		}
	}
	return std::shared_ptr<ExitNode>();
}

ExitNodePtr ControlFlowGraph::getExitNodeWithOneWayPredecessors(){
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		//msg("----------------------->>>>>>> node(%d): type %d, nPreds = %d\n", currentNode->id, currentNode->nodeType, currentNode->getPredecessors()->size());
		if(currentNode->nodeType == Exit_NODE && currentNode->instructions->size() < 15){
			intSetPtr exitNodePredecessors = currentNode->getPredecessors();
			for(std::set<int>::iterator pred_iter = exitNodePredecessors->begin() ; pred_iter != exitNodePredecessors->end() ; ++pred_iter){
				if(nodes->at(*pred_iter)->nodeType == ONE_WAY_NODE)
					return std::dynamic_pointer_cast<ExitNode>(currentNode);
			}
		}
	}
	return std::shared_ptr<ExitNode>();
}

void ControlFlowGraph::copyNodeCode(NodePtr sourceNode, NodePtr targetNode){
	if(targetNode->instructions->back()->type == DIRECT_JUMP)
		targetNode->instructions->pop_back();
	for(std::vector<InstructionPtr>::iterator inst_iter = sourceNode->instructions->begin() ; inst_iter != sourceNode->instructions->end() ; ++inst_iter){
		targetNode->instructions->push_back((*inst_iter)->deepcopy());
	}
}

void ControlFlowGraph::mergeIntoExitNode(NodePtr sourceNode, NodePtr targetNode){
	ExitNodePtr exitNode = std::make_shared<ExitNode>(sourceNode->id, sourceNode->startEA, targetNode->endEA, sourceNode->instructions);
	exitNode->predecessors = sourceNode->predecessors;
	if(!exitNode->instructions->empty() && exitNode->instructions->back()->type == DIRECT_JUMP)
		exitNode->instructions->pop_back();
	for(std::vector<InstructionPtr>::iterator inst_iter = targetNode->instructions->begin() ; inst_iter != targetNode->instructions->end() ; ++inst_iter){
		InstructionPtr instCopy = (*inst_iter)->deepcopy();
		instCopy->ContainingNodeId = exitNode->id;
		exitNode->instructions->push_back(instCopy);
	}
	nodes->operator[](exitNode->id) = exitNode;
}

void ControlFlowGraph::updateContiningNode(NodePtr node){
	for(std::vector<InstructionPtr>::iterator inst_iter = node->instructions->begin() ; inst_iter != node->instructions->end() ; ++inst_iter)
		(*inst_iter)->ContainingNodeId = node->id;
}

std::string ControlFlowGraph::to_json(){
	std::map<std::string, std::string> conditions_map;
	std::string nodes_json = "\"nodes\":[";
	int tag_index = 0;
	int current_max_index = nodes->size();
	for(std::map<int, NodePtr>::iterator node_iter = nodes->begin() ; node_iter != nodes->end() ; node_iter++){
		NodePtr current_node = node_iter->second;
		std::string current_node_json = "{\"id\":\"" + boost::lexical_cast<std::string>(current_node->id) + "\"";
		switch(current_node->nodeType){
		case TWO_WAY_NODE:
			{
				if(current_node->instructions->size() > 1){
					int condition_node_index = current_max_index++;
					current_node_json.append(", \"type\":\"Code\", \"successors\":[{\"node_id\":\""	+ boost::lexical_cast<std::string>(condition_node_index)
											  + "\", \"tag\":\"None\"}], " + current_node->instructions_json() + "}, ");
					current_node_json.append("{\"id\":\"" + boost::lexical_cast<std::string>(condition_node_index) + "\"");
				}
				current_node_json.append(", \"type\":\"Conditional\", \"successors\":[");	
				ConditionalJumpPtr last_inst = std::dynamic_pointer_cast<ConditionalJump>(current_node->instructions->back());
				std::string tag_name = "A" + boost::lexical_cast<std::string>(tag_index++);
				conditions_map[tag_name] = last_inst->condition->to_json();
				//conditions_map_json.append("{\"tag_name\":\"" + tag_name + "\", \"tag_condition\":" + last_inst->condition->to_json() + "}, ");
				TwoWayNodePtr currentTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(current_node);
				current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(currentTwoWayNode->trueSuccessorID) + "\", \"tag\":\"" + tag_name + "\"}, ");
				current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(currentTwoWayNode->falseSuccessorID) + "\", \"tag\":\"!" + tag_name + "\"}]}");
				nodes_json.append(current_node_json + ", ");
			}
			break;
		case N_WAY_NODE:
			{

				if(current_node->instructions->size() > 1){
					int switch_node_index = current_max_index++;
					current_node_json.append(", \"type\":\"Code\", \"successors\":[{\"node_id\":\""	+ boost::lexical_cast<std::string>(switch_node_index)
											  + "\", \"tag\":\"None\"}], " + current_node->instructions_json() + "}, ");
					current_node_json.append("{\"id\":\"" + boost::lexical_cast<std::string>(switch_node_index) + "\"");
				}

				std::vector<std::string> auxiliary_nodes_json;
				current_node_json.append(", \"type\":\"Switch\", \"successors\":[");
				N_WayNodePtr currentN_WayNode = std::dynamic_pointer_cast<N_WayNode>(current_node);
				IndirectTableJumpPtr switch_inst = std::dynamic_pointer_cast<IndirectTableJump>(current_node->instructions->back());
				ExpressionPtr switch_var = switch_inst->switchVariable;
				for(std::map<int, std::set<int>>::iterator succ_iter = switch_inst->nodeToCasesMap->begin() ; succ_iter != switch_inst->nodeToCasesMap->end() ; ++succ_iter){
					////////////////////
					ExpressionVectorPtr operands = std::make_shared<std::vector<ExpressionPtr>>();
					for(std::set<int>::iterator case_iter = succ_iter->second.begin() ; case_iter != succ_iter->second.end() ; ++case_iter){
						operands->push_back(std::make_shared<HighLevelCondition>("==", switch_var, std::make_shared<NumericConstant>(*case_iter)));
					}
					std::string tag_name = "A" + boost::lexical_cast<std::string>(tag_index++);
					conditions_map[tag_name] = ORExpression(operands, true).to_json();
					current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(succ_iter->first) + "\", \"tag\":\"" + tag_name + "\"}, ");
					////////////////////
					/*int last_index = succ_iter->second.size() - 1;
					for(std::set<int>::iterator case_iter = succ_iter->second.begin() ; case_iter != succ_iter->second.end() ; ++case_iter){
						std::string tag_name = "A" + boost::lexical_cast<std::string>(tag_index++);
						conditions_map[tag_name] = HighLevelCondition("==", switch_var, std::make_shared<NumericConstant>(*case_iter)).to_json();
						if(last_index == 0){
							current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(succ_iter->first) + "\", \"tag\":\"" + tag_name + "\"}, ");
						}
						else if(--last_index == 0){
							int aux_node_id = current_max_index++;
							current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(aux_node_id) + "\", \"tag\":\"" + tag_name + "\"}, ");
							auxiliary_nodes_json.push_back("{\"id\":\"" + boost::lexical_cast<std::string>(aux_node_id) + "\", \"type\":\"Code\", \"successors\":["
															+ "{\"node_id\":\"" + boost::lexical_cast<std::string>(succ_iter->first) + "\", \"tag\":\"None\"}], \"instructions\":[]}");
						}
						else{
							int aux_node_id = current_max_index++;
							current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(aux_node_id) + "\", \"tag\":\"" + tag_name + "\"}, ");
							auxiliary_nodes_json.push_back("{\"id\":\"" + boost::lexical_cast<std::string>(aux_node_id) + "\", \"type\":\"Code\", \"successors\":["
															+ "{\"node_id\":\"" + boost::lexical_cast<std::string>(current_max_index) + "\", \"tag\":\"None\"}], \"instructions\":[]}");
						}
					}*/
				}
				current_node_json = current_node_json.substr(0, current_node_json.length() - 2) + "]}";
				nodes_json.append(current_node_json + ", ");
				for(std::vector<std::string>::iterator aux_iter = auxiliary_nodes_json.begin() ; aux_iter != auxiliary_nodes_json.end() ; ++aux_iter){
					nodes_json.append(*aux_iter + ", ");
				}
			}
			break;
		default:
			{
				current_node_json.append(", \"type\":\"Code\", \"successors\":[");
				intSetPtr successor_ids = current_node->getSuccessors();
				std::set<int>::iterator succ_iter = successor_ids->begin();
				if(succ_iter != successor_ids->end())
					current_node_json.append("{\"node_id\":\"" + boost::lexical_cast<std::string>(*succ_iter) + "\", \"tag\":\"None\"}");
				current_node_json.append("], ");
				current_node_json.append(current_node->instructions_json() + "}");
				nodes_json.append(current_node_json + ", ");
			}
		}
	}
	if(nodes_json.at(nodes_json.length() - 1) == ' ' && nodes_json.at(nodes_json.length() - 2) == ',')
		nodes_json = nodes_json.substr(0, nodes_json.length() - 2) + "]";

	std::string conditions_map_json = "\"conditions_map\":[";
	for(std::map<std::string, std::string>::iterator cond_iter = conditions_map.begin(); cond_iter != conditions_map.end(); ++cond_iter)
		conditions_map_json.append("{\"tag_name\":\"" + cond_iter->first + "\", \"tag_condition\":" + cond_iter->second + "}, ");
	if(conditions_map_json.at(conditions_map_json.length() - 1) == ' ' && conditions_map_json.at(conditions_map_json.length() - 2) == ',')
		conditions_map_json = conditions_map_json.substr(0, conditions_map_json.length() - 2);
	conditions_map_json.append("]");
	
	return "{" + nodes_json + ", " + conditions_map_json + "}";
}
