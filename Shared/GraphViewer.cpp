////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//#include "GraphViewer.h"
//
//
//GraphViewer::GraphViewer(ControlFlowGraph* controlFlowGraph)
//{
//	functionName = controlFlowGraph->functionName;
//	initialize(controlFlowGraph);
//}
//
//
//GraphViewer::~GraphViewer(void)
//{
//	/*numberOfNodes = 0;
//	edgesMap.clear();
//	graph_text.clear();*/
//}
//
//
//void GraphViewer::show(){
//	HWND hwnd = NULL;
//	TForm *form = create_tform(functionName.c_str(), &hwnd);
//	if(hwnd != NULL){
//		netnode id;
//		id.create();
//		graph_viewer_t* gv = create_graph_viewer(form, id, callback, NULL, 0);
//		open_tform(form, FORM_MDI | FORM_TAB | FORM_MENU);
//		if(gv != NULL){
//			viewer_fit_window(gv);
//		}
//	}
//	else{
//		close_tform(form, 0);
//	}
//}
//
//void GraphViewer::initialize(ControlFlowGraph* controlFlowGraph){
//	numberOfNodes = controlFlowGraph->nodes->size();
//	graph_text.resize(numberOfNodes);
//	edgesMap.clear();
//	for(std::map<int, Node*>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; node_iter++){
//		Node* currentNode = node_iter->second;
//		graph_text[currentNode->id] = currentNode->getInstructionsCOLSTR();
//		switch(currentNode->nodeType){
//		case ONE_WAY_NODE:
//			{
//				OneWayNode* currentOneWayNode = (OneWayNode*)currentNode;
//				edgesMap[currentOneWayNode->id][currentOneWayNode->successorID] = NULL;
//			}
//			break;
//		case TWO_WAY_NODE:
//			{
//				TwoWayNode* currentTwoWayNode = (TwoWayNode*)currentNode;
//				edge_info_t* ei_true = new edge_info_t();
//				ei_true->color = 0x44FF55;
//				edgesMap[currentTwoWayNode->id][currentTwoWayNode->trueSuccessorID] = ei_true;
//				edge_info_t* ei_false = new edge_info_t();
//				ei_false->color = 0x0000FF;
//				edgesMap[currentTwoWayNode->id][currentTwoWayNode->falseSuccessorID] = ei_false;
//			}
//			break;
//		case N_WAY_NODE:
//			{
//				N_WayNode* currentN_WayNode = (N_WayNode*)currentNode;
//				for(std::vector<int>::iterator succ_iter = currentN_WayNode->successorIDs->begin() ; succ_iter != currentN_WayNode->successorIDs->end() ; succ_iter++){
//					edgesMap[currentN_WayNode->id][*succ_iter] = NULL;
//				}
//			}
//			break;
//		}
//	}
//}
//
//
//int idaapi callback(void *, int code, va_list va){
//	
//	int result = 0;
//	switch(code){
//	case grcode_user_refresh:
//		{
//			mutable_graph_t* g = va_arg(va, mutable_graph_t*);
//			if(g->empty()){
//				g->resize(numberOfNodes);
//			}
//			
//			for(std::map<int, std::map<int, edge_info_t*>>::iterator edgeStart_iter = edgesMap.begin() ; edgeStart_iter != edgesMap.end() ; ++edgeStart_iter){
//				for(std::map<int, edge_info_t*>::iterator edgeEnd_iter = edgeStart_iter->second.begin() ; edgeEnd_iter != edgeStart_iter->second.end() ; ++edgeEnd_iter){
//					g->add_edge(edgeStart_iter->first, edgeEnd_iter->first, edgeEnd_iter->second);
//				}
//			}
//			result = true;
//		}
//		break;
//	case grcode_user_gentext:
//		{
//			//mutable_graph_t* g = va_arg(va, mutable_graph_t*);
//			//graph_text.resize(g->size());
//			//for(node_iterator iter = g->begin() ; iter != g->end() ; ++iter){
//			//	int index = *iter;
//			//	std::string nodeText = flowGraph->nodes->operator[](index)->getNodeCOLSTR();
//			//	graph_text[index] = nodeText.c_str();
//			//	/*char* textPtr = (*(flowGraph->nodes))[index]->getInstructionsCOLSTR();
//			//	graph_text[index] = textPtr;
//			//	free(textPtr);
//			//	textPtr = NULL;*/
//			//}
//			result = true;
//		}
//		break;
//	case grcode_user_text:
//		{
//			mutable_graph_t* g = va_arg(va, mutable_graph_t*);
//			int node = va_arg(va, int);
//			const char **text = va_arg(va, const char**);
//			bgcolor_t* bgcolor = va_arg(va, bgcolor_t*);
//			if(text == NULL)
//				msg("text is null\n");
//			*text = graph_text[node].c_str();
//			if(bgcolor != NULL){
//				*bgcolor = DEFCOLOR;
//			}
//			result = true;
//			qnotused(g);
//		}
//		break;
//	}
//	return result;
//}
