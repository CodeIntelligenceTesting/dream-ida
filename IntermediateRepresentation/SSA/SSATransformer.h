/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <stdio.h>

#include <stack>

#include <ida.hpp>
#include <idp.hpp>
#include <gdl.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/dominator_tree.hpp>

#include "../IRHeaders.h"

class Variable;

//typedef boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, boost::property<boost::vertex_index_t, std::size_t>, boost::no_property> Graph;
//typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
//typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//typedef boost::iterator_property_map<std::vector<Vertex>::iterator, IndexMap> PredMap;

//typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_weight_t, int>> GraphD;
//typedef boost::graph_traits<GraphD>::vertex_descriptor VertexD;
//typedef boost::property_map<GraphD, boost::vertex_index_t>::type IndexMapD;
//
//typedef std::pair<int, int> Edge;

class SSATransformer
{
public:
	SSATransformer(ControlFlowGraphPtr _controlFlowGraph);
	virtual ~SSATransformer(void);

	void transformIntoSSA();
	void subscriptMemoryLocations();

	ControlFlowGraphPtr getControlFlowGraph(){return controlFlowGraph;};
	definitionsMapPtr getFlagDefinitionsMap(){return flagDefinitionsMap;};
	definitionsMapPtr getDefinitionsMap(){return definitionsMap;};
	usesMapPtr getUsesMap(){return usesMap;};
	void unifyAliasRegisters();
private:
	std::map<Variable, std::set<Variable>> aliasRegisters;
	std::vector<int> idom;
	std::vector<std::set<int>> dom_tree;
	int** distances_dom_tree;
	ControlFlowGraphPtr controlFlowGraph;
	//std::set<int> additionalEntryBlocks;

	std::map<int, std::set<std::string>> A_orig;
	std::set<std::string> definedVariablesNames;
	std::map<std::string, ExpressionPtr> definedVariablesObjects;
	std::map<int, std::set<int>> dominanceFrontier;

	std::map<std::string, int> SSACount;
	std::map<std::string, std::stack<int>> SSAStack;

	definitionsMapPtr definitionsMap;
	usesMapPtr usesMap;
	definitionsMapPtr flagDefinitionsMap;

	void buildDominatorTree();
	int** getShortestPaths();
	std::set<int> computeDominanceFrontier(int nodeId, std::set<int>& handledNodes);
	void computeDominanceFrontierMap();
	bool pathExistsDomTree(int from_nodeId, int to_nodeId);
	
	void compute_Aorig_No_Pointers();
	void compute_Aorig_Pointers();
	void addUsedRetaledToDefinedRegisters();
	void assignNamesToMemoryLocations();
	void place_Phi_Functions();
	void renameVariables(int nodeId, bool includePointers);
	void updateSwitchVariableSubscript();
	void getInstructionsByAddress(int instructionAddress, std::vector<InstructionPtr>& matchingInstructions, int startingNodeId);
	void clearUsedDataStructures();
	RegisterPtr getUsedRegister(Variable var);
	RegisterPtr getSmallestAlias(std::vector<RegisterPtr> aliasRegisters);
	/*void setDefinedVariablesTypes();
	TypePtr getTypeFromUses(const std::string& name, int subscript);*/
};

