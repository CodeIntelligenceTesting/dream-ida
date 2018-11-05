/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <map>
#include <set>

#include <ida.hpp>
#include <idp.hpp>
#include <funcs.hpp>
#include <gdl.hpp>
#include <segment.hpp>
//#include <allins.hpp>

#include "../IRHeaders.h"
#include "InstructionHandler.h"
#include "../ControlFlowGraph.h"
#include "../SSA/SSATransformer.h"
#include "../../Shared/GraphViewer.h"
#include "../IRSharedPointers.h"

#include "../../DataFlowAnalysis/DataStructures/Variable.h"

class IRBuilder
{
public:
	IRBuilder(void);
	virtual ~IRBuilder(void);

	bool buildIntermediateRepresentation(ea_t address);
	bool buildIRNoCalls(ea_t address);
	void subscriptMemoryLocations();

	ControlFlowGraphPtr getControlFlowGraph(){return controlFlowGraph;};
	definitionsMapPtr getDefinitionsMap(){return ssaTransformer->getDefinitionsMap();};
	definitionsMapPtr getFlagDefinitionsMap(){return ssaTransformer->getFlagDefinitionsMap();};
	usesMapPtr getUsesMap(){return ssaTransformer->getUsesMap();};
	//std::set<Variable>* getPhiVariableSet(){return ssaTransformer->getPhiVariableSet();};

	void drawGraph();
	void unifyAliasRegisters();
	std::string functionName;
private:
	ControlFlowGraphPtr controlFlowGraph;
	InstructionHandlerPtr iHandler;
	SSATransformerPtr ssaTransformer;
	bool hasMultipleHeaders(flowChartPtr flow_chart);
	//int getMaximumFPUIndex(qflow_chart_t* flow_chart);
	//Node* build_CFG_Node(int nodeId, std::vector<Instruction*>* inst_list, qflow_chart_t* flow_chart);
};
