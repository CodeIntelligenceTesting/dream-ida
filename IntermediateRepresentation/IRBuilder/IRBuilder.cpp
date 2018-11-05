////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "IRBuilder.h"


IRBuilder::IRBuilder(void)
{
	iHandler = NULL;
	ssaTransformer = NULL;
	controlFlowGraph = NULL;
}


IRBuilder::~IRBuilder(void)
{
	//delete iHandler; iHandler = NULL;
	//delete ssaTransformer; ssaTransformer = NULL;
	//delete controlFlowGraph; controlFlowGraph = NULL;
}

bool IRBuilder::buildIntermediateRepresentation(ea_t address){
	segment_t* seg = getseg(address);
	if(seg->type != SEG_CODE || !isCode(get_flags_novalue(address))){
		warning("Not code, please put the mouse cursor inside the function to be decompiled.");
		return false;
	}
	else{
		func_t* pfn = get_func(address);
		// TODO: demangle function name
		qstring func_name;
		get_func_name2(&func_name, pfn->startEA);
		const char* funcName = func_name.c_str();
		// char funcName[MAXSTR];
		// get_func_name(pfn->startEA, funcName, MAXSTR);
		// functionName = std::string(funcName);
		flowChartPtr flow_chart = std::make_shared<qflow_chart_t>(funcName, pfn, pfn->startEA, pfn->endEA, FC_PREDS/*CHART_WINGRAPH*/);
		iHandler = std::make_shared<InstructionHandler>(flow_chart);
		if(hasMultipleHeaders(flow_chart))
			return false;
		
		controlFlowGraph = std::make_shared<ControlFlowGraph>(std::string(funcName));
		qvector<qbasic_block_t>::iterator bb_iter;
		//std::vector<Instruction*>* inst_list;
		for(bb_iter = flow_chart->blocks.begin() ; bb_iter != flow_chart->blocks.end(); bb_iter++){
			InstructionVectorPtr inst_list = std::make_shared<std::vector<InstructionPtr>>();
			for(ea_t addr = bb_iter->startEA ; addr < bb_iter->endEA ; addr++){
				flags_t flags = get_flags_novalue(addr);
				if(isHead(flags) && isCode(flags)){
					int numberOfInstructionBeforeAdding = inst_list->size();
					decode_insn(addr);
					//msg("decoding at %x\n", addr);
					iHandler->updateInstructionList(inst_list);
					int numberOfAddedInstructions = inst_list->size() - numberOfInstructionBeforeAdding;
					
					//msg("numberOfAddedInstructions = %d\n\t", numberOfAddedInstructions);
					int t = 0;
					if(numberOfAddedInstructions > 0){
						for(int i = inst_list->size() - numberOfAddedInstructions ; i < inst_list->size(); ++i){
							//msg("%d >= %d, ", i, inst_list->size() - numberOfAddedInstructions);
							inst_list->operator[](i)->address = addr;
						}
					}
					//msg("\n");
				}
			}
			//msg("------------\n");
			int nodeId = std::distance(flow_chart->blocks.begin(), bb_iter);
			/*if(flow_chart->blocks[nodeId].pred.empty()){
				FlagPtr dfFlag = std::make_shared<Flag>(CPU_FLAG, DF);
				NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
				InstructionPtr dfInitInst = std::make_shared<Assignment>(dfFlag, zero);
				inst_list->insert(inst_list->begin(), dfInitInst);
			}*/
			controlFlowGraph->addNode(nodeId, inst_list, *bb_iter);
			if(controlFlowGraph->nodes->at(nodeId) == NULL)
				return false;
		}
		
		controlFlowGraph->doneAddingNodes();
		ssaTransformer = std::make_shared<SSATransformer>(controlFlowGraph);
		ssaTransformer->transformIntoSSA();

	}
	return true;
}

bool IRBuilder::buildIRNoCalls(ea_t address){
	segment_t* seg = getseg(address);
	if(seg->type != SEG_CODE || !isCode(get_flags_novalue(address))){
		warning("Not code, please put the mouse cursor inside the function to be decompiled.");
		return false;
	}
	else{
		func_t* pfn = get_func(address);
		qstring func_name;
		get_func_name2(&func_name, pfn->startEA);
		const char* funcName = func_name.c_str();
		// get_func_name(pfn->startEA, funcName, MAXSTR);
		// functionName = std::string(funcName);
		flowChartPtr flow_chart = std::make_shared<qflow_chart_t>(funcName, pfn, pfn->startEA, pfn->endEA, FC_PREDS/*CHART_WINGRAPH*/);
		iHandler = std::make_shared<InstructionHandler>(flow_chart);
		if(hasMultipleHeaders(flow_chart))
			return false;
		
		controlFlowGraph = std::make_shared<ControlFlowGraph>(std::string(funcName));
		qvector<qbasic_block_t>::iterator bb_iter;
		//std::vector<Instruction*>* inst_list;
		for(bb_iter = flow_chart->blocks.begin() ; bb_iter != flow_chart->blocks.end(); bb_iter++){
			InstructionVectorPtr inst_list = std::make_shared<std::vector<InstructionPtr>>();
			for(ea_t addr = bb_iter->startEA ; addr < bb_iter->endEA ; addr++){
				flags_t flags = get_flags_novalue(addr);
				if(isHead(flags) && isCode(flags)){
					int numberOfInstructionBeforeAdding = inst_list->size();
					decode_insn(addr);
					//msg("decoding at %x\n", addr);
					iHandler->updateInstructionList(inst_list);
					int numberOfAddedInstructions = inst_list->size() - numberOfInstructionBeforeAdding;
					
					//msg("numberOfAddedInstructions = %d\n\t", numberOfAddedInstructions);
					int t = 0;
					if(numberOfAddedInstructions > 0){
						for(int i = inst_list->size() - numberOfAddedInstructions ; i < inst_list->size(); ++i){
							//msg("%d >= %d, ", i, inst_list->size() - numberOfAddedInstructions);
							inst_list->operator[](i)->address = addr;
						}
					}
					//msg("\n");
				}
			}
			//msg("------------\n");
			int nodeId = std::distance(flow_chart->blocks.begin(), bb_iter);
			if(flow_chart->blocks[nodeId].pred.empty()){
				FlagPtr dfFlag = std::make_shared<Flag>(CPU_FLAG, DF);
				NumericConstantPtr zero = std::make_shared<NumericConstant>(0);
				InstructionPtr dfInitInst = std::make_shared<Assignment>(dfFlag, zero);
				inst_list->insert(inst_list->begin(), dfInitInst);
			}
			controlFlowGraph->addNode(nodeId, inst_list, *bb_iter);
		}
		
		controlFlowGraph->doneAddingNodes();
		ssaTransformer = std::make_shared<SSATransformer>(controlFlowGraph);
		ssaTransformer->transformIntoSSA();

		//for(std::map<int, NodePtr>)

	}
	return true;
}

void IRBuilder::subscriptMemoryLocations(){
	ssaTransformer->subscriptMemoryLocations();
}


void IRBuilder::unifyAliasRegisters(){
	ssaTransformer->unifyAliasRegisters();
}

void IRBuilder::drawGraph(){
	/*GraphViewer gv(controlFlowGraph);
	gv.show();*/
}

bool IRBuilder::hasMultipleHeaders(flowChartPtr flow_chart){
	int numHeaderNodes = 0;
	qvector<qbasic_block_t>::iterator bb_iter;
	for(bb_iter = flow_chart->blocks.begin() ; bb_iter != flow_chart->blocks.end(); bb_iter++){
		if(bb_iter->pred.size() == 0){
			numHeaderNodes++;
		}
	}
	msg("\nHas %d headers\n", numHeaderNodes);
	return numHeaderNodes > 1;
}

//Node* IRBuilder::build_CFG_Node(int nodeId, std::vector<Instruction*>* inst_list, qflow_chart_t* flow_chart){
//	qbasic_block_t currentBlock = flow_chart->blocks[nodeId];
//	NodeType nodeType;
//	switch(currentBlock.succ.size()){
//	case 0:
//		return new ExitNode(nodeId, currentBlock.startEA, currentBlock.endEA, inst_list);
//	case 1:
//		return new OneWayNode(nodeId, currentBlock.startEA, currentBlock.endEA, inst_list, currentBlock.succ[0]);
//	case 2:
//		{
//			int trueSuccessorID = currentBlock.succ[0];
//			int falseSuccessorID = currentBlock.succ[1];
//			ConditionalJump* cond_inst = dynamic_cast<ConditionalJump*>(inst_list->back());
//			if(cond_inst != NULL){
//				if(cond_inst->target != trueSuccessorID){
//					int tmp = trueSuccessorID;
//					trueSuccessorID = falseSuccessorID;
//					falseSuccessorID = tmp;
//				}
//				return new TwoWayNode(nodeId, currentBlock.startEA, currentBlock.endEA, inst_list, trueSuccessorID, falseSuccessorID);
//			}
//			else{
//				msg("ERROR: last instruction in a two way node is not ConditionalJump\n");
//				return NULL;
//			}
//		}
//		break;
//	default:
//		{
//			std::vector<int>* successorIDs = new std::vector<int>();
//			for(qvector<int>::iterator iter = currentBlock.succ.begin() ; iter != currentBlock.succ.end() ; iter++){
//				successorIDs->push_back(*iter);
//			}
//			return new N_WayNode(nodeId, currentBlock.startEA, currentBlock.endEA, inst_list, successorIDs);
//		}
//	}
//}

//int IRBuilder::getMaximumFPUIndex(qflow_chart_t* flow_chart){
//	int maxFPUIndex = -1;
//	qvector<qbasic_block_t>::iterator bb_iter;
//	for(bb_iter = flow_chart->blocks.begin() ; bb_iter != flow_chart->blocks.end() ; bb_iter++){
//		for(ea_t addr = bb_iter->startEA ; addr < bb_iter->endEA ; addr++){
//			flags_t flags = get_flags_novalue(addr);
//			if(isHead(flags) && isCode(flags)){
//				decode_insn(addr);
//				for(int opIndex = 0 ; opIndex < UA_MAXOP ; opIndex++){
//					op_t op_i = cmd.Operands[opIndex];
//					if(op_i.type == o_void){
//						break;
//					}
//					else if(op_i.type == o_fpreg){
//						if(op_i.reg > maxFPUIndex){
//							maxFPUIndex = op_i.reg;
//						}
//					}
//				}
//			}
//		}
//	}
//	return maxFPUIndex;
//}
