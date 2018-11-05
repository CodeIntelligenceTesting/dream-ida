////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "SSATransformer.h"

#include "../../DataFlowAnalysis/DataStructures/Variable.h"
#include "../../DataFlowAnalysis/Algorithms/DataFlowAlgorithm.h"
#include "../../TypeAnalysis/Types/TopType.h"
#include "../../Shared/ExpressionUtilities.h"
#include "../../Shared/InstructionUtilities.h"

typedef boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, boost::property<boost::vertex_index_t, std::size_t>, boost::no_property> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
typedef boost::iterator_property_map<std::vector<Vertex>::iterator, IndexMap> PredMap;

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_weight_t, int>> GraphD;
typedef boost::graph_traits<GraphD>::vertex_descriptor VertexD;
typedef boost::property_map<GraphD, boost::vertex_index_t>::type IndexMapD;

typedef std::pair<int, int> Edge;

SSATransformer::SSATransformer(ControlFlowGraphPtr _controlFlowGraph)
{
	controlFlowGraph = _controlFlowGraph;
	idom.resize(controlFlowGraph->nodes->size());
	dom_tree.resize(controlFlowGraph->nodes->size());
	definitionsMap = std::make_shared<std::map<std::string, std::map<int, InstructionPtr>>>();
	usesMap = std::make_shared<std::map<std::string, std::map<int, std::vector<InstructionPtr>>>>();
	flagDefinitionsMap = std::make_shared<std::map<std::string, std::map<int, InstructionPtr>>>();
	distances_dom_tree = NULL;
	/*for(std::map<int, Node*>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		int blockId = node_iter->first;
		if(controlFlowGraph->getPredecessors(blockId).empty() && blockId != 0){
			additionalEntryBlocks.insert(blockId);
		}
	}*/
}


SSATransformer::~SSATransformer(void)
{
	if(distances_dom_tree != NULL){
		for(unsigned int i = 0 ; i < controlFlowGraph->nodes->size() ; i++){
			delete[] distances_dom_tree[i];
		}
		delete[] distances_dom_tree;
	}
}


void SSATransformer::transformIntoSSA(){
	buildDominatorTree();
	distances_dom_tree = getShortestPaths();
	computeDominanceFrontierMap();
	compute_Aorig_No_Pointers();
	addUsedRetaledToDefinedRegisters();
	place_Phi_Functions();
	
	for(std::set<std::string>::iterator variable_name_iter = definedVariablesNames.begin() ; variable_name_iter != definedVariablesNames.end() ; ++variable_name_iter){
		SSACount[*variable_name_iter] = 0;
		SSAStack[*variable_name_iter].push(0);
	}

	renameVariables(0, false);
	/*for(std::set<int>::iterator entry_iter = additionalEntryBlocks.begin() ; entry_iter != additionalEntryBlocks.end() ; ++entry_iter){
		renameVariables(*entry_iter, false);
	}*/
	//setDefinedVariablesTypes();
	updateSwitchVariableSubscript();
	clearUsedDataStructures();
	//unifyAliasRegisters();
}


void SSATransformer::subscriptMemoryLocations(){
	compute_Aorig_Pointers();
	place_Phi_Functions();
	
	for(std::set<std::string>::iterator variable_name_iter = definedVariablesNames.begin() ; variable_name_iter != definedVariablesNames.end() ; ++variable_name_iter){
		SSACount[*variable_name_iter] = 0;
		SSAStack[*variable_name_iter].push(0);
	}

	assignNamesToMemoryLocations();
	renameVariables(0, true);

	//setDefinedVariablesTypes();
	clearUsedDataStructures();
}


 void SSATransformer::buildDominatorTree(){
	std::vector<Edge> edges;
	
	intToNodeMapPtr nodes = controlFlowGraph->nodes;
	for(std::map<int, NodePtr>::iterator iter = nodes->begin() ; iter != nodes->end() ; iter++){
		NodePtr currentNode = iter->second;
		switch(currentNode->nodeType){
		case ONE_WAY_NODE:
			{
				OneWayNodePtr currentOneWayNode = std::dynamic_pointer_cast<OneWayNode>(currentNode);
				edges.push_back(Edge(currentOneWayNode->id, currentOneWayNode->successorID));
			}
			break;
		case TWO_WAY_NODE:
			{
				TwoWayNodePtr currentTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(currentNode);
				edges.push_back(Edge(currentTwoWayNode->id, currentTwoWayNode->trueSuccessorID));
				edges.push_back(Edge(currentTwoWayNode->id, currentTwoWayNode->falseSuccessorID));
			}
			break;
		case N_WAY_NODE:
			{
				N_WayNodePtr currentN_WayNode = std::dynamic_pointer_cast<N_WayNode>(currentNode);
				for(std::set<int>::iterator succ_iter = currentN_WayNode->successorIDs->begin(); succ_iter != currentN_WayNode->successorIDs->end() ; succ_iter++){
					edges.push_back(Edge(currentN_WayNode->id, *succ_iter));
				}
			}
			break;
		}
	}

	Graph boostGraph(edges.begin(), edges.end(), nodes->size());

	std::vector<Vertex> domTreePredVector;
	IndexMap indexMap(boost::get(boost::vertex_index, boostGraph));
	boost::graph_traits<Graph>::vertex_iterator iter, iterEnd;
	int j = 0;
	for(boost::tie(iter, iterEnd) = boost::vertices(boostGraph) ; iter != iterEnd ; ++iter, ++j){
		boost::put(indexMap, *iter, j);
	}
	domTreePredVector = std::vector<Vertex>(boost::num_vertices(boostGraph), boost::graph_traits<Graph>::null_vertex());
	PredMap domTreePredMap = boost::make_iterator_property_map(domTreePredVector.begin(), indexMap);
	boost::lengauer_tarjan_dominator_tree(boostGraph, boost::vertex(0, boostGraph), domTreePredMap);

	for(boost::tie(iter, iterEnd) = boost::vertices(boostGraph) ; iter != iterEnd ; ++iter){
		if(boost::get(domTreePredMap,*iter) != boost::graph_traits<Graph>::null_vertex()){
			int dominatingNode = boost::get(indexMap, boost::get(domTreePredMap, *iter));
			int dominatedNode = boost::get(indexMap, *iter);
			idom[dominatedNode] = dominatingNode;
			dom_tree[dominatingNode].insert(dominatedNode);
		}
		else{
			idom[boost::get(indexMap, *iter)] = -1;
		}
	}

	//------------------------------------------------------
	/*std::vector<Edge> domTreeEdges;
	int num_edges_dom_tree = 0;
	for(boost::tie(iter, iterEnd) = boost::vertices(boostGraph) ; iter != iterEnd ; ++iter){
		if(boost::get(domTreePredMap,*iter) != boost::graph_traits<Graph>::null_vertex()){
			domTreeEdges.push_back(Edge(boost::get(indexMap, boost::get(domTreePredMap, *iter)), boost::get(indexMap, *iter)));
			num_edges_dom_tree++;
		}
	}
	
	int * weights = new int[num_edges_dom_tree];
	for(int i = 0 ; i < num_edges_dom_tree ; i++){
		weights[i] = 1;
	}

	GraphD domTreeGraph(domTreeEdges.begin(), domTreeEdges.end(), weights, nodes->size());*/
	/*int ** distances = getShortestPaths(domTreeGraph);
	int numOfVertices = boost::num_vertices(domTreeGraph);
	for(int i = 0 ; i < numOfVertices ; i++){
		for(int j = 0 ; j < numOfVertices ; j++){
			if(distances[i][j] != (std::numeric_limits<int>::max)())
				msg("d(%d,%d)=%d\n", i, j, distances[i][j]);	
		}
	}*/
}


int** SSATransformer::getShortestPaths(){
	int numOfVertices = controlFlowGraph->nodes->size();
	int** distances = new int*[numOfVertices];
	for(int i = 0 ; i < numOfVertices ; i++){
		distances[i] = new int[numOfVertices];
		for(int j = 0 ; j < numOfVertices ; j++){
			distances[i][j] = -1;
		}
	}

	std::vector<Edge> domTreeEdges;
	int num_edges_dom_tree = 0;
	for(unsigned int nodeId = 0 ; nodeId < idom.size() ; nodeId++){
		int idominatingNode = idom[nodeId];
		if(idominatingNode != -1){
			domTreeEdges.push_back(Edge(idominatingNode, nodeId));
			num_edges_dom_tree++;
		}
	}
	
	/*int * weights = new int[num_edges_dom_tree];
	for(int i = 0 ; i < num_edges_dom_tree ; i++){
		weights[i] = 1;
	}*/
	if(num_edges_dom_tree > 0){
		std::vector<int> weightsVector(num_edges_dom_tree, 1);
		int* weights = &weightsVector[0];

		GraphD domTreeGraph(domTreeEdges.begin(), domTreeEdges.end(), weights, numOfVertices);

		std::vector<VertexD> p(numOfVertices);
		std::vector<int> d(numOfVertices);
		boost::graph_traits<GraphD>::vertex_iterator iter1, iterEnd1, iter2, iterEnd2;
		for(boost::tie(iter1, iterEnd1) = boost::vertices(domTreeGraph) ; iter1 != iterEnd1 ; ++iter1){
			boost::dijkstra_shortest_paths(domTreeGraph, *iter1, boost::predecessor_map(&p[0]).distance_map(&d[0]));
			for(boost::tie(iter2, iterEnd2) = boost::vertices(domTreeGraph) ; iter2 != iterEnd2 ; ++iter2){
				distances[*iter1][*iter2] = (d[*iter2] != (std::numeric_limits<int>::max)()) ? d[*iter2] : -1;
			}
		}
	}
	return distances;
}


std::set<int> SSATransformer::computeDominanceFrontier(int nodeId, std::set<int>& handledNodes){
	std::set<int> DF;
	intSetPtr successors = controlFlowGraph->getSuccessors(nodeId);
	for(std::set<int>::iterator succ_iter = successors->begin() ; succ_iter != successors->end() ; ++succ_iter){
		if(nodeId != idom[*succ_iter]){
			DF.insert(*succ_iter);
		}
	}
	for(std::set<int>::iterator dominated_iter = dom_tree[nodeId].begin() ; dominated_iter != dom_tree[nodeId].end() ; ++dominated_iter){
		std::set<int> DF_child;
		if(handledNodes.find(*dominated_iter) == handledNodes.end()){
			DF_child = computeDominanceFrontier(*dominated_iter, handledNodes);
			handledNodes.insert(*dominated_iter);
			dominanceFrontier[*dominated_iter] = DF_child;
		}
		else{
			DF_child = dominanceFrontier[*dominated_iter];
		}
		
		for(std::set<int>::iterator child_iter = DF_child.begin() ; child_iter != DF_child.end() ; ++child_iter){
			if(!pathExistsDomTree(nodeId, *child_iter) || nodeId == *child_iter){
				DF.insert(*child_iter);
			}
		}
	}
	return DF;
}


void SSATransformer::computeDominanceFrontierMap(){
	std::set<int> handledNodes;
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		int nodeId = node_iter->first;
		if(handledNodes.find(nodeId) == handledNodes.end()/*dominanceFrontier[nodeId].empty()*/){
			dominanceFrontier[nodeId] = computeDominanceFrontier(nodeId, handledNodes);
			handledNodes.insert(nodeId);
		}
	}
}


bool SSATransformer::pathExistsDomTree(int from_nodeId, int to_nodeId){
	return distances_dom_tree[from_nodeId][to_nodeId] != -1;
}


void SSATransformer::compute_Aorig_No_Pointers(){
	for(unsigned int nodeId = 0 ; nodeId < controlFlowGraph->nodes->size() ; nodeId++){
		std::set<std::string> nodeVariablesNames;
		InstructionVectorPtr node_instructions = controlFlowGraph->nodes->at(nodeId)->instructions;//(*(controlFlowGraph->nodes))[nodeId]->instructions;
		for(std::vector<InstructionPtr>::iterator inst_iter = node_instructions->begin() ; inst_iter != node_instructions->end() ; ++inst_iter){
			InstructionPtr currentInst = *inst_iter;
		
			if(currentInst->type == ASSIGNMENT){
			
				AssignmentPtr currentAssignment = std::dynamic_pointer_cast<Assignment>(currentInst);
				ExpressionPtr lhsOperand = currentAssignment->lhsOperand;
			
				if(lhsOperand->type != POINTER){
					
					if(lhsOperand->type == BIT_CONCATENATION){
							
						FlagConcatenationPtr flagConcatExp = std::dynamic_pointer_cast<FlagConcatenation>(lhsOperand);
						intToFlagMapPtr flags = flagConcatExp->flagMap;
						for(std::map<int, FlagPtr>::iterator flag_iter = flags->begin() ; flag_iter != flags->end() ; ++flag_iter){
							std::string flagVariableName = flag_iter->second->getName();
							if(nodeVariablesNames.find(flagVariableName) == nodeVariablesNames.end()){
								nodeVariablesNames.insert(flagVariableName);
								if(definedVariablesNames.find(flagVariableName) == definedVariablesNames.end()){
									definedVariablesNames.insert(flagVariableName);
									definedVariablesObjects[flagVariableName] = flag_iter->second->deepcopy();
								}
							}
						}
					}
					else {
						std::string lhsOperandVariableName = lhsOperand->getName();
						if(!lhsOperandVariableName.empty() && nodeVariablesNames.find(lhsOperandVariableName) == nodeVariablesNames.end()){
							nodeVariablesNames.insert(lhsOperandVariableName);
							if(definedVariablesNames.find(lhsOperandVariableName) == definedVariablesNames.end()){
								definedVariablesNames.insert(lhsOperandVariableName);
								definedVariablesObjects[lhsOperandVariableName] = lhsOperand->deepcopy();
							}
						}
					}
				}
			}
			else if(currentInst->type == CALL){
				CallPtr currentCall = std::dynamic_pointer_cast<Call>(currentInst);
				ExpressionVectorPtr returns = currentCall->returns;
				for(std::vector<ExpressionPtr>::iterator ret_iter = returns->begin() ; ret_iter != returns->end() ; ++ret_iter){
					std::string retVariableName = (*ret_iter)->getName();
					if(nodeVariablesNames.find(retVariableName) == nodeVariablesNames.end()){
						nodeVariablesNames.insert(retVariableName);
						if(definedVariablesNames.find(retVariableName) == definedVariablesNames.end()){
							definedVariablesNames.insert(retVariableName);
							definedVariablesObjects[retVariableName] = (*ret_iter)->deepcopy();
						}
					}
				}
			}
			else if(currentInst->type == FLAG_MACRO){
				FlagSetPtr definedFlags = std::dynamic_pointer_cast<FlagMacro>(currentInst)->definedFlags;//((FlagMacro*)currentInst)->definedFlags;
				for(std::set<FlagPtr>::iterator flag_iter = definedFlags->begin() ; flag_iter != definedFlags->end() ; ++flag_iter){
					std::string flagVariableName = (*flag_iter)->getName();
					if(nodeVariablesNames.find(flagVariableName) == nodeVariablesNames.end()){
						nodeVariablesNames.insert(flagVariableName);
						if(definedVariablesNames.find(flagVariableName) == definedVariablesNames.end()){
							definedVariablesNames.insert(flagVariableName);
							definedVariablesObjects[flagVariableName] = (*flag_iter)->deepcopy();
						}
					}
				}
			}
		}
		A_orig[nodeId] = nodeVariablesNames;
	}
}


void SSATransformer::compute_Aorig_Pointers(){
	for(unsigned int nodeId = 0 ; nodeId < controlFlowGraph->nodes->size() ; nodeId++){
		std::set<std::string> nodeVariablesNames;
		InstructionVectorPtr node_instructions = controlFlowGraph->nodes->at(nodeId)->instructions;//(*(controlFlowGraph->nodes))[nodeId]->instructions;
		for(std::vector<InstructionPtr>::iterator inst_iter = node_instructions->begin() ; inst_iter != node_instructions->end() ; ++inst_iter){
			InstructionPtr currentInst = *inst_iter;
			if(currentInst->type == ASSIGNMENT){
				ExpressionPtr lhsOpExp = std::dynamic_pointer_cast<Assignment>(currentInst)->lhsOperand;//((Assignment*)currentInst)->lhsOperand;
				std::string lhsOpName = lhsOpExp->getExpressionString();
				if(lhsOpExp->type == POINTER && nodeVariablesNames.find(lhsOpName) == nodeVariablesNames.end()){
					nodeVariablesNames.insert(lhsOpName);
					if(definedVariablesNames.find(lhsOpName) == definedVariablesNames.end()){
						definedVariablesNames.insert(lhsOpName);
						definedVariablesObjects[lhsOpName] = lhsOpExp->deepcopy();
					}
				}
			}
			else if(currentInst->type == CALL){
				ExpressionVectorPtr returns = std::dynamic_pointer_cast<Call>(currentInst)->returns;//((Call*)currentInst)->returns;
				if(returns->size() != 0){
					for(std::vector<ExpressionPtr>::iterator ret_iter = returns->begin() ; ret_iter != returns->end() ; ++ret_iter){
						ExpressionPtr retExp = *ret_iter;
						std::string retName = retExp->getExpressionString();
						if(retExp->type == POINTER && nodeVariablesNames.find(retName) == nodeVariablesNames.end()){
							nodeVariablesNames.insert(retName);
							if(definedVariablesNames.find(retName) == definedVariablesNames.end()){
								definedVariablesNames.insert(retName);
								definedVariablesObjects[retName] = retExp->deepcopy();
							}
						}
					}
				}
			}
		}
		A_orig[nodeId] = nodeVariablesNames;
	}
}


void SSATransformer::addUsedRetaledToDefinedRegisters(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		InstructionVectorPtr inst_list = node_iter->second->instructions;
		for(std::vector<InstructionPtr>::iterator inst_iter = inst_list->begin() ; inst_iter != inst_list->end() ; ++inst_iter){
			std::vector<ExpressionPtr> usedElements;
			(*inst_iter)->getUsedElements(usedElements);
			if(!usedElements.empty()){
				for(std::vector<ExpressionPtr>::iterator use_iter = usedElements.begin() ; use_iter != usedElements.end() ; ++use_iter){
			
					ExpressionPtr usedElement = *use_iter;
					if(usedElement->type == REGISTER && definedVariablesNames.find(usedElement->getName()) == definedVariablesNames.end()){
					
						std::set<std::string>* relatedRegistersNames = &(std::dynamic_pointer_cast<Register>(usedElement)->relatedRegistersNames);//&(((Register*)usedElement)->relatedRegistersNames);
						for(std::set<std::string>::iterator reg_iter = relatedRegistersNames->begin() ; reg_iter != relatedRegistersNames->end() ; ++reg_iter){
							if(definedVariablesNames.find(*reg_iter) != definedVariablesNames.end()){
								definedVariablesNames.insert(usedElement->getName());
								definedVariablesObjects[usedElement->getName()] = usedElement->deepcopy();
								break;
							}
						}
					}
				}
			}
		}
	}
}


void SSATransformer::assignNamesToMemoryLocations(){
	std::vector<ExpressionPtr> defs, uses;
	for(unsigned int nodeId = 0 ; nodeId < controlFlowGraph->nodes->size() ; nodeId++){
		InstructionVectorPtr node_instructions = controlFlowGraph->nodes->at(nodeId)->instructions;//(*(controlFlowGraph->nodes))[nodeId]->instructions;
		for(std::vector<InstructionPtr>::iterator inst_iter = node_instructions->begin() ; inst_iter != node_instructions->end() ; ++inst_iter){
			InstructionPtr currentInst = *inst_iter;
			
			defs.clear();
			currentInst->getDefinedElements(defs);
			if(!defs.empty()){
				for(std::vector<ExpressionPtr>::iterator def_iter = defs.begin() ; def_iter != defs.end() ; ++def_iter){
					ExpressionPtr defExp = *def_iter;
					if(defExp->type == POINTER){
						//msg("before defExp->getName()= (%s)\n", defExp->getName().c_str());
						std::dynamic_pointer_cast<PointerExp>(defExp)->hasName = true;
						//msg("after defExp->getName()= (%s)\n", defExp->getName().c_str());
					}
				}
			}

			uses.clear();
			currentInst->getUsedElements(uses);
			if(!uses.empty()){
				for(std::vector<ExpressionPtr>::iterator use_iter = uses.begin() ; use_iter != uses.end() ; ++use_iter){
					ExpressionPtr useExp = *use_iter;
					if(useExp->type == POINTER){
						//msg("before useExp->getName()= (%s)\n", useExp->getName().c_str());
						std::dynamic_pointer_cast<PointerExp>(useExp)->hasName = true;
						//msg("after useExp->getName()= (%s)\n", useExp->getName().c_str());
					}
				}
			}
		}
	}
}


void SSATransformer::place_Phi_Functions(){
	std::map<std::string, std::set<int>> defsites;
	std::map<std::string, std::set<int>> A_phi;
	
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		int nodeId = node_iter->first;
		for(std::set<std::string>::iterator variable_name_iter = A_orig[nodeId].begin() ; variable_name_iter != A_orig[nodeId].end() ; ++variable_name_iter){
			defsites[*variable_name_iter].insert(nodeId);
		}
	}

	for(std::set<std::string>::iterator variable_name_iter = definedVariablesNames.begin() ; variable_name_iter != definedVariablesNames.end() ; ++variable_name_iter){
		std::string variableName = *variable_name_iter;
		std::set<int> definingNodes = defsites[variableName];
		while(!definingNodes.empty()){
			int defNodeId = *(definingNodes.begin());
			definingNodes.erase(definingNodes.begin());
			for(std::set<int>::iterator df_iter = dominanceFrontier[defNodeId].begin() ; df_iter != dominanceFrontier[defNodeId].end() ; ++df_iter){
				int dominanceFrontierNodeId = *df_iter;
				if(A_phi[variableName].find(dominanceFrontierNodeId) == A_phi[variableName].end()){
					InstructionVectorPtr inst_list = controlFlowGraph->nodes->at(dominanceFrontierNodeId)->instructions;//(*(controlFlowGraph->nodes))[dominanceFrontierNodeId]->instructions;
					Phi_FunctionPtr newPhiFunction = std::make_shared<Phi_Function>(definedVariablesObjects[variableName]->deepcopy(), controlFlowGraph->getPredecessors(dominanceFrontierNodeId)->size());
					newPhiFunction->ContainingNodeId = dominanceFrontierNodeId;
					
					//this was to avoid edx = phi() followed by dl = phi(): TODO better solution is needed
					int offset = 0;
					for(std::vector<InstructionPtr>::iterator phi_iter = inst_list->begin() ; phi_iter != inst_list->end() ; ++phi_iter){
						if((*phi_iter)->type == PHI_FUNCTION)
							offset++;
						else
							break;
					}
					
					inst_list->insert(inst_list->begin() + offset, newPhiFunction);
					
					A_phi[variableName].insert(dominanceFrontierNodeId);
					if(A_orig[dominanceFrontierNodeId].find(variableName) == A_orig[dominanceFrontierNodeId].end()){
						definingNodes.insert(dominanceFrontierNodeId);
					}
				}
			}
		}
	}
}


void SSATransformer::renameVariables(int nodeId, bool includePointers){
	std::map<std::string, int> numberOfDefinitions;
	for(std::set<std::string>::iterator variable_name_iter = definedVariablesNames.begin() ; variable_name_iter != definedVariablesNames.end() ; ++variable_name_iter){
		numberOfDefinitions[*variable_name_iter] = 0;
	}

	NodePtr node = controlFlowGraph->nodes->at(nodeId);//(*(controlFlowGraph->nodes))[nodeId];
	for(std::vector<InstructionPtr>::iterator inst_iter = node->instructions->begin() ; inst_iter != node->instructions->end() ; ++inst_iter){
		InstructionPtr inst = *inst_iter;
		if(inst->type != PHI_FUNCTION){
			std::vector<ExpressionPtr> uses;
			inst->getUsedElements(uses);
			
			if(!uses.empty()){
				for(std::vector<ExpressionPtr>::iterator use_iter = uses.begin() ; use_iter != uses.end() ; ++use_iter){
					ExpressionPtr useExp = *use_iter;
					std::string useName = useExp->getName();
					if(!useName.empty()){
						if(SSAStack.find(useName) != SSAStack.end()){
							int useSubscript = SSAStack[useName].top();
							useExp->subscript = useSubscript;
							usesMap->operator[](useName).operator[](useSubscript).push_back(inst);
							//usesMap[useName][useSubscript].push_back(inst);
						}
					}
				}
			}
		}
		
		std::vector<ExpressionPtr> defs;
		inst->getDefinedElements(defs);
		if(!defs.empty()){
			for(std::vector<ExpressionPtr>::iterator def_iter = defs.begin() ; def_iter != defs.end() ; ++def_iter){
				ExpressionPtr defExp = *def_iter;
				std::string defName = defExp->getName();
				if(!defName.empty()){
					if(SSAStack.find(defName) != SSAStack.end()){
						numberOfDefinitions[defName]++;
						SSACount[defName]++;
						int defSubscript = SSACount[defName];
						SSAStack[defName].push(defSubscript);
						defExp->subscript = defSubscript;

						if(inst->type == PHI_FUNCTION){
							Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(inst);
							phiFunction->target->phiSourceBlockId = nodeId;
							//phiVariables.insert(Variable(phiFunction->target->getName(), phiFunction->target->subscript));
							//phiFunction->definingNode[defName][defSubscript].insert(nodeId);
						}

						definitionsMap->operator[](defName).operator[](defSubscript) = inst;
						//definitionsMap[defName][defSubscript] = inst;
						if(defExp->type == FLAG){
							flagDefinitionsMap->operator[](defName).operator[](defSubscript) = inst;
							//flagDefinitionsMap[defName][defSubscript] = inst;
						}

						if(defExp->type == REGISTER){
							std::set<Variable> otherUses;
							RegisterPtr regExp = std::dynamic_pointer_cast<Register>(defExp);
							for(std::set<std::string>::iterator related_reg_iter = regExp->relatedRegistersNames.begin() ; related_reg_iter != regExp->relatedRegistersNames.end() ; ++related_reg_iter){
								std::string relatedRegisterName = *related_reg_iter;
								if(definedVariablesNames.find(relatedRegisterName) != definedVariablesNames.end()){
									numberOfDefinitions[relatedRegisterName]++;
									SSACount[relatedRegisterName]++;
									defSubscript = SSACount[relatedRegisterName];
									SSAStack[relatedRegisterName].push(defSubscript);
									definitionsMap->operator[](relatedRegisterName).operator[](defSubscript) = inst;
									otherUses.insert(Variable(relatedRegisterName, defSubscript));
									//definitionsMap[relatedRegisterName][defSubscript] = inst;
								}
							}
							if(otherUses.size() > 0){
								//otherUses.insert(Variable(regExp->name, regExp->subscript));
								aliasRegisters[Variable(regExp->name, regExp->subscript)] = otherUses;
							}
						}
					}
				}
			}
			//delete defs;
		}
	}
	intSetPtr successors = controlFlowGraph->getSuccessors(nodeId);
	for(std::set<int>::iterator succ_iter = successors->begin() ; succ_iter != successors->end() ; ++succ_iter){
		NodePtr successorNode = controlFlowGraph->nodes->at(*succ_iter);//(*(controlFlowGraph->nodes))[*succ_iter];
		for(std::vector<InstructionPtr>::iterator inst_iter = successorNode->instructions->begin() ; inst_iter != successorNode->instructions->end() ; ++inst_iter){
			InstructionPtr inst = *inst_iter;
			if(inst->type == PHI_FUNCTION){
				Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(inst);
				if((includePointers && phiFunction->target->type == POINTER) || (!includePointers && phiFunction->target->type != POINTER)){
					std::string phiTargetName = phiFunction->target->getName();
					int useSubscript = SSAStack[phiTargetName].top();
					for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
						ExpressionPtr argExp = *arg_iter;
						if(argExp->subscript == NO_SUBSCRIPT){
							argExp->subscript = useSubscript;
							argExp->phiSourceBlockId = nodeId;
							//phiVariables.insert(Variable(argExp->getName(), argExp->subscript));
							//phiFunction->definingNode[argExp->getName()][useSubscript].insert(nodeId);
							break;
						}
					}
					usesMap->operator[](phiTargetName).operator[](useSubscript).push_back(inst);
				}
			}
			else{
				break;
			}
		}
	}

	for(std::set<int>::iterator dominated_node_iter = dom_tree[nodeId].begin() ; dominated_node_iter != dom_tree[nodeId].end() ; ++dominated_node_iter){
		renameVariables(*dominated_node_iter, includePointers);
	}
	for(std::map<std::string, int>::iterator def_num_iter = numberOfDefinitions.begin() ; def_num_iter != numberOfDefinitions.end() ; ++def_num_iter){
		for(int i = 0 ; i < def_num_iter->second ; ++i){
			SSAStack[def_num_iter->first].pop();
		}
	}
}


void SSATransformer::updateSwitchVariableSubscript(){
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; ++node_iter){
		NodePtr currentNode = node_iter->second;
		if(currentNode->getSuccessors()->size() > 2){
			InstructionPtr lastInstruction = currentNode->instructions->back();
			if(lastInstruction->type == INDIRECT_TABLE_JUMP){
				IndirectTableJumpPtr switchInstruction = std::dynamic_pointer_cast<IndirectTableJump>(lastInstruction);
				ExpressionPtr switchVariable = switchInstruction->switchVariable;
				if(switchVariable.get() != NULL){
					//get start inst
					//update subscript
					//msg("switch instruction: %s --> %x\n", switchInstruction->getInstructionString().c_str(), switchInstruction->startAddressOfSwitchIdiom);
					std::vector<InstructionPtr> matchingInstructions;
					getInstructionsByAddress(switchInstruction->startAddressOfSwitchIdiom, matchingInstructions, node_iter->first);
					for(std::vector<InstructionPtr>::iterator it = matchingInstructions.begin() ; it != matchingInstructions.end() ; ++it){
						InstructionPtr matchingInstruction = *it;
						std::vector<ExpressionPtr> usedElements;
						matchingInstruction->getUsedElements(usedElements);
						for(std::vector<ExpressionPtr>::iterator use_iter = usedElements.begin() ; use_iter != usedElements.end() ; ++use_iter){
							ExpressionPtr usedElement = *use_iter;
							if(usedElement->getName().compare(switchVariable->getName()) == 0 && usedElement->subscript != switchVariable->subscript && usedElement->type == switchVariable->type){
								//update usesMap and definitionsMap
								//think about related registers
								
								
								/////////////////////TEMP////////////////////
								//std::vector<InstructionPtr> vec = usesMap->operator[](usedElement->getName()).operator[](usedElement->subscript);
								//msg("uses(%s)\n", usedElement->getExpressionString().c_str());
								//for(std::vector<InstructionPtr>::iterator vi = vec.begin() ; vi != vec.end() ; ++vi)
								//	msg("\t%s\n", (*vi)->getInstructionString().c_str());
								/////////////////////TEMP////////////////////
								//vec = usesMap->operator[](switchVariable->getName()).operator[](switchVariable->subscript);
								//msg("uses(%s)_<switchVariable>\n", switchVariable->getExpressionString().c_str());
								//for(std::vector<InstructionPtr>::iterator vi = vec.begin() ; vi != vec.end() ; ++vi)
								//	msg("\t%s\n", (*vi)->getInstructionString().c_str());
								std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator use_name_iter = usesMap->find(switchVariable->getName());
								if(use_name_iter != usesMap->end()){
									std::map<int, std::vector<InstructionPtr>>::iterator use_subscript_iter = use_name_iter->second.find(switchVariable->subscript);
									if(use_subscript_iter != use_name_iter->second.end()){
										while(true){
											std::vector<InstructionPtr>::iterator useToRemove = std::find(use_subscript_iter->second.begin(), use_subscript_iter->second.end(), switchInstruction);
											if(useToRemove == use_subscript_iter->second.end())
												break;
											else{
												use_subscript_iter->second.erase(useToRemove);
												usesMap->operator[](usedElement->getName()).operator[](usedElement->subscript).push_back(switchInstruction);
											}
										}
									}
								}
								switchVariable->subscript = usedElement->subscript;
								return;
							}
						}
					}
				}
			}
		}
	}
}


void SSATransformer::getInstructionsByAddress(int instructionAddress, std::vector<InstructionPtr>& matchingInstructions, int startingNodeId){
	NodePtr currentNode = controlFlowGraph->nodes->at(startingNodeId);
	for(std::vector<InstructionPtr>::reverse_iterator inst_iter = currentNode->instructions->rbegin() ; inst_iter != currentNode->instructions->rend() ; ++inst_iter){
		InstructionPtr currentInstruction = *inst_iter;
		if(currentInstruction->address == instructionAddress)
			matchingInstructions.push_back(currentInstruction);
	}
	if(matchingInstructions.empty()){
		intSetPtr predecessorNodes = controlFlowGraph->getPredecessors(currentNode->id);
		for(std::set<int>::iterator pred_iter = predecessorNodes->begin() ; pred_iter != predecessorNodes->end() ; ++pred_iter)
			getInstructionsByAddress(instructionAddress, matchingInstructions, *pred_iter);
	}
}


void SSATransformer::clearUsedDataStructures(){
	definedVariablesNames.clear();
	definedVariablesObjects.clear();
	A_orig.clear();
	SSAStack.clear();
	SSACount.clear();
}


void SSATransformer::unifyAliasRegisters(){
	DataFlowAlgorithm dfa(controlFlowGraph, definitionsMap, flagDefinitionsMap, usesMap);
	for(std::map<Variable, std::set<Variable>>::iterator it = aliasRegisters.begin() ; it != aliasRegisters.end() ; ++it){
		Variable orig_var = it->first;
		InstructionPtr def_inst = dfa.getDefiningInstruction(orig_var.name, orig_var.subscript);
		if(!isInstructionPointerNull(def_inst)){
			//msg("defining instruction: %s\n", def_inst->getInstructionString().c_str());
			std::vector<RegisterPtr> aliasedUses;
			ExpressionPtr defined_expr = def_inst->getDefinedExpression(orig_var.name, orig_var.subscript);
			if(defined_expr->type == REGISTER && defined_expr->isSameVariable(orig_var.name, orig_var.subscript)){
				RegisterPtr def_reg = std::dynamic_pointer_cast<Register>(defined_expr);
				if(isExpressionPointerNull(getUsedRegister(orig_var))){
					aliasedUses.push_back(def_reg);
				}
			}
			RegisterPtr orig_reg = getUsedRegister(orig_var);
			if(!isExpressionPointerNull(orig_reg)){
				//msg("alias register: %s\n", orig_var.toString().c_str());
				aliasedUses.push_back(orig_reg);
			}
			for(std::set<Variable>::iterator it2 = it->second.begin() ; it2 != it->second.end() ; ++it2){
				RegisterPtr aliasReg = getUsedRegister(*it2);
				if(!isExpressionPointerNull(aliasReg)){
					//msg("\t: %s\n", (*it2).toString().c_str());
					aliasedUses.push_back(aliasReg);
				}
			}
			if(!aliasedUses.empty()){
				//msg("num of aliases %d\n", aliasedUses.size());
				RegisterPtr finalReg = getSmallestAlias(aliasedUses);
				if(finalReg != orig_reg){
					//	definitionsMap->operator[](orig_var.name).operator[](orig_var.subscript);
					def_inst->replaceDefinition(orig_var.name, orig_var.subscript, finalReg->deepcopy());
					//dfa.updateDefinitionsMap(finalReg, def_inst);
					std::vector<InstructionPtr>* usingInstructions = dfa.getUsingInstructions(orig_var.name, orig_var.subscript);
					for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
						InstructionPtr use_inst = *use_iter;
						use_inst->replaceUse(orig_var.name, orig_var.subscript, finalReg->deepcopy());
						dfa.addInstructionToUsesMap(finalReg, use_inst);
					}
					usingInstructions->clear();
				}

				for(std::vector<RegisterPtr>::iterator alias_iter = aliasedUses.begin() ; alias_iter != aliasedUses.end() ; ++alias_iter){
					RegisterPtr aliasReg = *alias_iter;
					if(aliasReg != finalReg){
						std::vector<InstructionPtr>* usingInstructions = dfa.getUsingInstructions(aliasReg->name, aliasReg->subscript);
						for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
							InstructionPtr use_inst = *use_iter;
							use_inst->replaceUse(aliasReg->name, aliasReg->subscript, finalReg->deepcopy());
							dfa.addInstructionToUsesMap(finalReg, use_inst);
						}
						usingInstructions->clear();
					}
				}
			}
		}
	}
	
}

RegisterPtr SSATransformer::getUsedRegister(Variable var){
	if(usesMap->find(var.name) != usesMap->end()){
		if(usesMap->operator[](var.name).find(var.subscript) != usesMap->operator[](var.name).end()){
			std::vector<InstructionPtr>* used_instructions = &(usesMap->operator[](var.name).operator[](var.subscript));
			if(!used_instructions->empty()){
				//msg("testing %s_%d: %d uses\n", var.name.c_str(), var.subscript, used_instructions->size());
				//InstructionPtr use_inst = used_instructions->front();
				std::vector<ExpressionPtr> usedRegs;
				for(std::vector<InstructionPtr>::iterator use_it = used_instructions->begin() ; use_it != used_instructions->end() ; ++use_it){
					//msg("   is used in %s\n", (*iit)->getInstructionString().c_str());
					(*use_it)->getUsesOfExpression(var.name, var.subscript, usedRegs);
					if(!usedRegs.empty())
						break;
				}
				
				//use_inst->getUsesOfExpression(var.name, var.subscript, usedRegs);
				//if(!usedRegs.empty()){
				return std::dynamic_pointer_cast<Register>(*usedRegs.begin());
				//}
			}
		}
	}
	return std::shared_ptr<Register>();
}

RegisterPtr SSATransformer::getSmallestAlias(std::vector<RegisterPtr> aliasRegisters){
	RegisterPtr smallestReg = aliasRegisters.front();
	for(std::vector<RegisterPtr>::iterator rit = aliasRegisters.begin() ; rit != aliasRegisters.end() ; ++rit){
		if((*rit)->size_in_bytes < smallestReg->size_in_bytes){
			smallestReg = *rit;
		}
	}
	return smallestReg;
}


//void SSATransformer::setDefinedVariablesTypes(){
//	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator def_name_iter = definitionsMap->begin() ; def_name_iter != definitionsMap->end() ; ++def_name_iter){
//		std::string def_name = def_name_iter->first;
//		for(std::map<int, InstructionPtr>::iterator def_subscript_iter = def_name_iter->second.begin() ; def_subscript_iter != def_name_iter->second.end() ; ++def_subscript_iter){
//			int def_subscript = def_subscript_iter->first;
//			InstructionPtr definingInstruction = def_subscript_iter->second;
//			ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(def_name, def_subscript);
//			if(definedExpression.get() != NULL){
//				TypePtr type = definedExpression->expressionType;
//				if(type->type == TOP_TYPE)
//					type = getTypeFromUses(def_name, def_subscript);
//				definedExpression->expressionType = type;
//				msg("%s (%s)\n", definedExpression->getExpressionString().c_str(), definedExpression->expressionType->getTypeCOLSTR().c_str());
//			}
//			//msg("Expression %s_%d is %s\n", def_name.c_str(), def_subscript, (definedExpression.get() == NULL ? "null" : "not null"));
//			
//		}
//	}
//}
//
//TypePtr SSATransformer::getTypeFromUses(const std::string& name, int subscript){
//	std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator use_name_iter = usesMap->find(name);
//	if(use_name_iter != usesMap->end()){
//		std::map<int, std::vector<InstructionPtr>>::iterator use_subscript_iter = use_name_iter->second.find(subscript);
//		if(use_subscript_iter != use_name_iter->second.end()){
//			for(std::vector<InstructionPtr>::iterator use_inst_iter = use_subscript_iter->second.begin() ; use_inst_iter != use_subscript_iter->second.end() ; ++use_inst_iter){
//				std::vector<ExpressionPtr> uses;
//				(*use_inst_iter)->getUsesOfExpression(name, subscript, uses);
//				for(std::vector<ExpressionPtr>::iterator use_iter = uses.begin() ; use_iter != uses.end() ; ++use_iter){
//					TypePtr useType = (*use_iter)->getExpresstionType();
//					//msg("\t%s\n", (*use_iter)->getInstructionString().c_str());
//					if(useType->type != TOP_TYPE)
//						return useType;
//					//msg("\t\t%s\n", useType->getTypeCOLSTR().c_str());
//				}
//			}
//		}
//	}
//	/*std::vector<InstructionPtr>* usingInstructions
//	for(std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator use_name_iter = usesMap->begin() ; use_name_iter != usesMap->end() ; ++use_name_iter){
//		std::string use_name = use_name_iter->first;
//		for(std::map<int, std::vector<InstructionPtr>>::iterator use_subscript_iter = use_name_iter->second.begin() ; use_subscript_iter != use_name_iter->second.end() ; ++use_subscript_iter){
//			int use_subscript = use_subscript_iter->first;
//			for(std::vector<InstructionPtr>::iterator use_iter = use_subscript_iter->second.begin() ; use_iter != use_subscript_iter->second.end() ; ++use_iter){
//				TypePtr useType = (*use_iter)->getUsedExpression(use_name, use_subscript)->expressionType;
//				if(useType->type != TOP_TYPE)
//					msg("\t%s --> %s\n", (*use_iter)->getInstructionString().c_str(), useType->getTypeCOLSTR().c_str());
//			}
//		}
//	}*/
//	return std::make_shared<TopType>();
//}

