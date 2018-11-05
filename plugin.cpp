////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <string>
#include <fstream>
#include <stdio.h>

#include <boost/filesystem.hpp>

#include <ida.hpp>
#include <idp.hpp>
#include <graph.hpp>
#include <kernwin.hpp>
#include <loader.hpp>



#include "IntermediateRepresentation/IRBuilder/IRBuilder.h"
#include "DataFlowAnalysis/DataFlowAnalyzer.h"
#include "DataFlowAnalysis/Algorithms/ConditionCodePropagation.h"
#include "ControlFlowAnalysis/ControlFlowAnalyzer.h"
#include "TypeAnalysis/TypeAnalyzer/TypeAnalyzer.h"
#include "CodeGeneration/DecompiledCodeGenerator.h"

#include "ControlFlowAnalysis/GraphTraversal/GraphTraversal.h"


static std::map<int, std::map<int, edge_info_t*>> edgesMap;
static std::vector<std::string> graph_text;
static int numberOfNodes;
static TForm *form = NULL;

static void showGraph(const ControlFlowGraphPtr &controlFlowGraph);
static void initializeGraph(const ControlFlowGraphPtr &controlFlowGraph);
static void freeAllocatedResources();
static int idaapi callback(void *, int code, va_list va);


int idaapi IDAP_init(void)
{
	return ( callui(ui_get_hwnd).vptr != NULL || is_idaq() ) ? PLUGIN_OK : PLUGIN_SKIP;
}

void idaapi IDAP_term(void)
{
	freeAllocatedResources();
	return;
}

const char *dialog = 
	"STARTITEM 0\n"
	"Decompilation options\n\n"
	"<Output Folder:F:1:64::>\n"
	"<##Output##JSON File:R>\n"
	"<Show Decompiled Code:R>>\n"
	"<##Scope##Single Function:R>\n"
	"<All Function:R>>\n";

const int OUT_JSON = 0;
const int OUT_DECOMPILED_CODE = 1;

const int SINGLE_FUNCTION = 0;
const int ALL_FUNCTIONS = 1;

bool hasMultipleHeaders(qflow_chart_t *flow_chart){
	int numHeaderNodes = 0;
	qvector<qbasic_block_t>::iterator bb_iter;
	for(bb_iter = flow_chart->blocks.begin() ; bb_iter != flow_chart->blocks.end(); bb_iter++){
		if(bb_iter->pred.size() == 0){
			numHeaderNodes++;
		}
	}
	return numHeaderNodes > 1;
}

bool hasNWayNodes(qflow_chart_t *flow_chart){
	qvector<qbasic_block_t>::iterator bb_iter;
	for(bb_iter = flow_chart->blocks.begin() ; bb_iter != flow_chart->blocks.end(); bb_iter++){
		if(bb_iter->succ.size() > 2){
			return true;
		}
	}
	return false;
}

bool hasOneBasicBlock(qflow_chart_t *flow_chart){
	return flow_chart->blocks.size() <= 1;
}

std::string to_json(ea_t functionAddress){
	IRBuilder irBuilder;
	if(irBuilder.buildIntermediateRepresentation(functionAddress)){
		TypeAnalyzer typeAnalyzer(irBuilder.getDefinitionsMap(), irBuilder.getUsesMap());
		typeAnalyzer.analyzeTypes();
		DataFlowAnalyzer dataFlowAnalyzer(irBuilder.getControlFlowGraph(), irBuilder.getDefinitionsMap(), irBuilder.getFlagDefinitionsMap(), irBuilder.getUsesMap());
		dataFlowAnalyzer.applyDataFlowAnalysisFirstRound();
		irBuilder.subscriptMemoryLocations();
		typeAnalyzer.analyzeTypes();
		dataFlowAnalyzer.applyDataFlowAnalysisSecondRound();

		/*ConditionCodePropagation ccp(irBuilder.getControlFlowGraph(), irBuilder.getDefinitionsMap(), irBuilder.getFlagDefinitionsMap(), irBuilder.getUsesMap());
		ccp.replace_remaining_flag_macros();*/

		dataFlowAnalyzer.translateOutOfSSA(typeAnalyzer.getTypeMap());
		//ControlFlowAnalyzer controlFlowAnalyzer(irBuilder.getControlFlowGraph());
		//controlFlowAnalyzer.applyControlFlowAnalysis();
		std::string function_name_json = "\"function_name\":\"" + irBuilder.functionName + "\"";
		std::string arguments_json = "\"arguments\": [";
		std::vector<ExpressionPtr>::iterator arg_iter;
		for(arg_iter = dataFlowAnalyzer.functionArguments->begin(); arg_iter != dataFlowAnalyzer.functionArguments->end(); ++arg_iter){
			arguments_json.append((*arg_iter)->to_json());
			if(arg_iter + 1 != dataFlowAnalyzer.functionArguments->end())
				arguments_json.append(", ");
		}
		arguments_json.append("]");

		std::string json = "{" + function_name_json + ", " + arguments_json + ", \"cfg\":" + irBuilder.getControlFlowGraph()->to_json() + "}";
		return json;
	}
	return "ERROR";
}

std::string to_json_new(ea_t functionAddress){
	IRBuilder irBuilder;
	if(irBuilder.buildIntermediateRepresentation(functionAddress)){
		TypeAnalyzer typeAnalyzer(irBuilder.getDefinitionsMap(), irBuilder.getUsesMap());
		typeAnalyzer.analyzeTypes();
		DataFlowAnalyzer dataFlowAnalyzer(irBuilder.getControlFlowGraph(), irBuilder.getDefinitionsMap(), irBuilder.getFlagDefinitionsMap(), irBuilder.getUsesMap());
		dataFlowAnalyzer.applyDataFlowAnalysisFirstRound();
		irBuilder.subscriptMemoryLocations();
		//typeAnalyzer.analyzeTypes();
		dataFlowAnalyzer.applyDataFlowAnalysisSecondRound();
		irBuilder.unifyAliasRegisters();
		/*ConditionCodePropagation ccp(irBuilder.getControlFlowGraph(), irBuilder.getDefinitionsMap(), irBuilder.getFlagDefinitionsMap(), irBuilder.getUsesMap());
		ccp.replace_remaining_flag_macros();*/
		dataFlowAnalyzer.translateOutOfSSA(typeAnalyzer.getTypeMap());
		//ControlFlowAnalyzer controlFlowAnalyzer(irBuilder.getControlFlowGraph());
		//controlFlowAnalyzer.applyControlFlowAnalysis();
		std::string function_name_json = "\"function_name\":\"" + irBuilder.functionName + "\"";
		std::string arguments_json = "\"arguments\": [";
		std::vector<ExpressionPtr>::iterator arg_iter;
		for(arg_iter = dataFlowAnalyzer.functionArguments->begin(); arg_iter != dataFlowAnalyzer.functionArguments->end(); ++arg_iter){
			arguments_json.append((*arg_iter)->to_json());
			if(arg_iter + 1 != dataFlowAnalyzer.functionArguments->end())
				arguments_json.append(", ");
		}
		arguments_json.append("]");

		std::string json = "{" + function_name_json + ", " + arguments_json + ", \"cfg\":" + irBuilder.getControlFlowGraph()->to_json() + "}";
		return json;
	}
	return "ERROR";
}

// Structure to keep all information about the our viewer
struct viewer_info_t
{
  TForm *form;
  TCustomControl *cv;
  TCustomControl *codeview;
  strvec_t sv;
  viewer_info_t(TForm *f) : form(f), cv(NULL) {}
};

int idaapi ui_callback(void *ud, int code, va_list va){
	viewer_info_t *vi = (viewer_info_t*)ud;
	switch ( code )
	{
	case ui_tform_invisible:
		{
			TForm *f = va_arg(va, TForm *);
			if ( f == vi->form )
			{
				delete vi;
				unhook_from_notification_point(HT_UI, ui_callback, NULL);
			}
		}
		break;
	}
	return 0;
}

void showDecompiledCode(const stringVectorPtr &codeLines, const std::map<int, int> &lineNumberToLabelMap, const std::set<int> &usedLabels){
	HWND hwnd = NULL;
	TForm *form = create_tform("DREAM", &hwnd);
	if ( hwnd == NULL )
	{
		warning("Could not create decompiled code view window\n"
			"perhaps it is open?\n"
			"Switching to it.");
		form = find_tform("DREAM");
		if ( form != NULL )
			switchto_tform(form, true);
		return;
	}
	// allocate block to hold info about our sample view
	viewer_info_t *vi = new viewer_info_t(form);
	// prepare the data to display. we could prepare it on the fly too.
	// but for that we have to use our own custom place_t class decendant.
	for(int line = 0 ; line < codeLines->size() ; ++line){
		std::map<int, int>::const_iterator lableLine_iter = lineNumberToLabelMap.find(line);
		if(lableLine_iter != lineNumberToLabelMap.end() && usedLabels.find(lableLine_iter->second) != usedLabels.end())
			vi->sv.push_back(simpleline_t(getCOLSTR("Node_" + boost::lexical_cast<std::string>(lableLine_iter->second) + ":", LABEL_COLOR).c_str()));
		vi->sv.push_back(simpleline_t(codeLines->operator[](line).c_str()));
	}
	//for (std::vector<std::string>::const_iterator codeLine_iter = codeLines->begin() ; codeLine_iter != codeLines->end(); ++codeLine_iter)	{
	//	//vi->sv.push_back(simpleline_t("")); // add empty line
	//	vi->sv.push_back(simpleline_t(codeLine_iter->c_str()));
	//}
	// create two place_t objects: for the minimal and maximal locations
	simpleline_place_t s1;
	simpleline_place_t s2(vi->sv.size()-1);
	// create a custom viewer
	vi->cv = create_custom_viewer("", (TWinControl *)form, &s1, &s2, &s1, NULL, &vi->sv, NULL, vi);
	// set the handlers so we can communicate with it
	//set_custom_viewer_handlers(si->cv, ct_keyboard, ct_popup, NULL, ct_curpos, NULL, si);

	// create a code viewer container for the custom viewer
	vi->codeview = create_code_viewer(form, vi->cv, CDVF_LINEICONS);
	// set handlers for the code viewer
	// set_code_viewer_line_handlers(vi->codeview, lines_click, NULL, NULL, lines_icon, lines_linenum);
	// draw maximal 2 icons in the lines widget
	set_code_viewer_lines_icon_margin(vi->codeview, 2);

	// also set the ui event callback
	hook_to_notification_point(HT_UI, ui_callback, vi);
	// finally display the form on the screen
	open_tform(form, FORM_TAB|FORM_MENU|FORM_RESTORE|FORM_QWIDGET);
}

void idaapi IDAP_run(int arg)
{
   char out_path_c[QMAXPATH] = {0};
   ushort output = 0;
   ushort scope = 0;

   if (AskUsingForm_c(dialog, out_path_c, &output, &scope) != 1)
	   return;

	if(scope == ALL_FUNCTIONS && output == OUT_DECOMPILED_CODE){
		msg("Options combination is not supported\n");
		return;
	}
	if(scope == ALL_FUNCTIONS || output == OUT_JSON){
		struct stat s;
		if(stat(out_path_c, &s) == 0){
			if((s.st_mode & S_IFDIR) == 0){
				msg("%s is not a directory\n", out_path_c);
				return;
			}
		}
		else{
			msg("Error in the output path (%s)\n", out_path_c);
			return;
		}
	}

    boost::filesystem::path out_path(out_path_c);
	char dir_name_c[MAXSTR];
	get_root_filename(dir_name_c, MAXSTR);
	boost::filesystem::path dir_name(dir_name_c);
	boost::filesystem::path full_path = out_path / dir_name;
	if(scope == ALL_FUNCTIONS || output == OUT_JSON){
		if(boost::filesystem::exists(full_path)){
			msg("%s already exists.\n", full_path.string().c_str());
			return;
		} 
		boost::filesystem::create_directory(full_path);
	}
	if(scope == ALL_FUNCTIONS){
		int non_trivial_functions = 0;
		int num_funcs = 0;
		ea_t text_start = get_segm_by_name(".text")->startEA;
		ea_t text_end = get_segm_by_name(".text")->endEA;
		/*ea_t init_start = get_segm_by_name(".init")->startEA;
		ea_t init_end = get_segm_by_name(".init")->endEA;
		ea_t fini_start = get_segm_by_name(".fini")->startEA;
		ea_t fini_end = get_segm_by_name(".fini")->endEA;*/
		ea_t lastFun = get_screen_ea();

		std::ofstream logFile;
		boost::filesystem::path log_path = out_path / "log.txt";
		logFile.open(log_path.string(), std::ofstream::out | std::ofstream::app);

		std::ofstream blocksFile;
		boost::filesystem::path blocks_path = out_path / "blocks.csv";
		blocksFile.open(blocks_path.string(), std::ofstream::out | std::ofstream::app);

		int startF = lastFun != 0 ? get_func_num(lastFun) : 0;
		int loop_max = 0;

		for(int f = startF ; f < get_func_qty() ; ++f){
			func_t *pfn = getn_func(f);//get_func(get_screen_ea());
			//if((pfn->startEA >= text_start && pfn->startEA <= text_end) || (pfn->startEA >= init_start && pfn->startEA <= init_end) || (pfn->startEA >= fini_start && pfn->startEA <= fini_end)){
			if(pfn->startEA >= text_start && pfn->startEA < text_end){
				num_funcs++;
				qstring funcName_qstring;
				get_func_name2(&funcName_qstring, pfn->startEA);
				const char* funcName = funcName_qstring.c_str();
				if(std::string(funcName).compare("copy_internal") == 0) //[ switch.deep_copy()
					continue;
				logFile << funcName;
				logFile.flush();
				//continue;
				qflow_chart_t flow_chart(funcName, pfn, pfn->startEA, pfn->endEA, FC_PREDS/*CHART_WINGRAPH*/);
				if(!hasMultipleHeaders(&flow_chart) && !hasNWayNodes(&flow_chart)){
					if(!hasOneBasicBlock(&flow_chart))
						non_trivial_functions++;

					std::string json = to_json_new(pfn->startEA);
					if(json.compare("ERROR") != 0){
						std::ofstream myfile;
						boost::filesystem::path json_file = full_path / (std::string(funcName) + std::string(".json"));
						myfile.open(json_file.string());
						myfile << json.c_str();
						myfile.close();
						blocksFile << dir_name << "," << funcName << "," << flow_chart.blocks.size() << std::endl;
						blocksFile.flush();
					}
					loop_max++;
				}
				logFile << " ... OK\n";
				logFile.flush();
				/*if(loop_max == 120)
						break;*/
			}
			//decompileFunction(pfn->startEA);
			//msg("HexRays: #Lines = %d, #Gotos = %d\n", numberOfLines, numberOfGotos);
		}
		logFile.close();
		blocksFile.close();
		msg("total functions: %d\n", num_funcs);
		msg("non trivial functions: %d\n", non_trivial_functions);
		return;
	}
	else{
		msg("start\n");
		ea_t ea = get_screen_ea();
		IRBuilder irBuilder;

		func_t *pfn = get_func(ea);
		qstring funcName_qstring;
		get_func_name2(&funcName_qstring, pfn->startEA);
		const char* funcName = funcName_qstring.c_str();

		if(irBuilder.buildIntermediateRepresentation(ea)){
			//showGraph(irBuilder.getControlFlowGraph());return;
			TypeAnalyzer typeAnalyzer(irBuilder.getDefinitionsMap(), irBuilder.getUsesMap());
			typeAnalyzer.analyzeTypes();
			
			DataFlowAnalyzer dataFlowAnalyzer(irBuilder.getControlFlowGraph(), irBuilder.getDefinitionsMap(), irBuilder.getFlagDefinitionsMap(), irBuilder.getUsesMap());
			dataFlowAnalyzer.applyDataFlowAnalysisFirstRound();
			msg("First round Data-flow analysis\n");
			//showGraph(irBuilder.getControlFlowGraph());return;
			
			irBuilder.subscriptMemoryLocations();
			//typeAnalyzer.analyzeTypes();

			//showGraph(irBuilder.getControlFlowGraph());return;
			
			dataFlowAnalyzer.applyDataFlowAnalysisSecondRound();
			////ConditionCodePropagation ccp(irBuilder.getControlFlowGraph(), irBuilder.getDefinitionsMap(), irBuilder.getFlagDefinitionsMap(), irBuilder.getUsesMap());
			////ccp.replace_remaining_flag_macros();
			msg("finished Data-flow analysis\n");
			//showGraph(irBuilder.getControlFlowGraph());//return;
			irBuilder.unifyAliasRegisters();
			//msg("TYPE: %s\n", typeAnalyzer.getTypeMap()->getType("dl", 33)->getTypeCOLSTR().c_str());
			// showGraph(irBuilder.getControlFlowGraph());
			
			dataFlowAnalyzer.translateOutOfSSA(typeAnalyzer.getTypeMap());
			//msg("translated out of SSA\n");
			//ControlFlowAnalyzer controlFlowAnalyzer(irBuilder.getControlFlowGraph());
			//controlFlowAnalyzer.applyControlFlowAnalysis();
			if(output == OUT_JSON){
				//showGraph(irBuilder.getControlFlowGraph());
				std::string function_name_json = "\"function_name\":\"" + irBuilder.functionName + "\"";
				std::string arguments_json = "\"arguments\": [";
				std::vector<ExpressionPtr>::iterator arg_iter;
				for(arg_iter = dataFlowAnalyzer.functionArguments->begin(); arg_iter != dataFlowAnalyzer.functionArguments->end(); ++arg_iter){
					arguments_json.append((*arg_iter)->to_json());
					if(arg_iter + 1 != dataFlowAnalyzer.functionArguments->end())
						arguments_json.append(", ");
				}
				arguments_json.append("]");
				std::string json = "{" + function_name_json + ", " + arguments_json + ", \"cfg\":" + irBuilder.getControlFlowGraph()->to_json() + "}";
				std::ofstream myfile;
				
				boost::filesystem::path json_file = full_path / (std::string(funcName) + std::string(".json"));
				myfile.open(json_file.string());
				myfile << json.c_str();
				myfile.close();
			}
			else if(output == OUT_DECOMPILED_CODE){
				ControlFlowAnalyzer controlFlowAnalyzer(irBuilder.getControlFlowGraph());
				controlFlowAnalyzer.applyControlFlowAnalysis();
				DecompiledCodeGenerator decompiledCodeGenerator(irBuilder.getControlFlowGraph(), controlFlowAnalyzer.graphStructeringInfo, dataFlowAnalyzer.functionArguments, irBuilder.functionName);
				decompiledCodeGenerator.generateCodeLines(typeAnalyzer.getTypeMap());
				showDecompiledCode(decompiledCodeGenerator.codeLines, decompiledCodeGenerator.lineNumberToLabelMap, decompiledCodeGenerator.usedLabels);
			}
		}
		msg("\nDONE\n");
		return;
	}
}


char IDAP_comment[] = "This is REcompile/DREAM plug-in";
char IDAP_help[] = "REcompile/DREAM plugin";


char IDAP_name[] = "DREAM";

char IDAP_hotkey[] = "Alt-D";


plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,	// IDA version plug-in is written for
  0,						// Flags (see below)
  IDAP_init,				// Initialisation function
  IDAP_term,				// Clean-up function
  IDAP_run,					// Main plug-in body
  IDAP_comment,				// Comment � unused
  IDAP_help,				// As above � unused
  IDAP_name,				// Plug-in name shown in
							// Edit->Plugins menu
  IDAP_hotkey				// Hot key to run the plug-in
};

void initializeGraph(const ControlFlowGraphPtr &controlFlowGraph){
	freeAllocatedResources();
	numberOfNodes = controlFlowGraph->nodes->size();
	graph_text.resize(numberOfNodes);
	//msg(controlFlowGraph->to_json().c_str());
	for(std::map<int, NodePtr>::iterator node_iter = controlFlowGraph->nodes->begin() ; node_iter != controlFlowGraph->nodes->end() ; node_iter++){
		NodePtr currentNode = node_iter->second;
		graph_text[currentNode->id] = currentNode->getNodeCOLSTR();
		switch(currentNode->nodeType){
		case ONE_WAY_NODE:
			{
				OneWayNodePtr currentOneWayNode = std::dynamic_pointer_cast<OneWayNode>(currentNode);
				edgesMap[currentOneWayNode->id][currentOneWayNode->successorID] = NULL;
			}
			break;
		case TWO_WAY_NODE:
			{
				TwoWayNodePtr currentTwoWayNode = std::dynamic_pointer_cast<TwoWayNode>(currentNode);
				edge_info_t* ei_true = new edge_info_t();
				ei_true->color = 0x44FF55;
				edgesMap[currentTwoWayNode->id][currentTwoWayNode->trueSuccessorID] = ei_true;
				edge_info_t* ei_false = new edge_info_t();
				ei_false->color = 0x0000FF;
				edgesMap[currentTwoWayNode->id][currentTwoWayNode->falseSuccessorID] = ei_false;
			}
			break;
		case N_WAY_NODE:
			{
				N_WayNodePtr currentN_WayNode = std::dynamic_pointer_cast<N_WayNode>(currentNode);
				for(std::set<int>::iterator succ_iter = currentN_WayNode->successorIDs->begin() ; succ_iter != currentN_WayNode->successorIDs->end() ; succ_iter++){
					edgesMap[currentN_WayNode->id][*succ_iter] = NULL;
				}
			}
			break;
		}
	}
}

void showGraph(const ControlFlowGraphPtr &controlFlowGraph){
	initializeGraph(controlFlowGraph);
	HWND hwnd = NULL;
	form = create_tform(controlFlowGraph->functionName.c_str(), &hwnd);
	if(hwnd != NULL){
		netnode id;
		id.create();
		graph_viewer_t* gv = create_graph_viewer(form, id, callback, NULL, 0);
		open_tform(form, FORM_MDI | FORM_TAB | FORM_MENU);
		if(gv != NULL){
			viewer_fit_window(gv);
		}
	}
	else{
		close_tform(form, 0);
	}
}

int idaapi callback(void *, int code, va_list va){
	int result = 0;
	switch(code){
	case grcode_user_refresh:
		{
			mutable_graph_t* g = va_arg(va, mutable_graph_t*);
			if(g->empty()){
				g->resize(numberOfNodes);
			}
			
			for(std::map<int, std::map<int, edge_info_t*>>::iterator edgeStart_iter = edgesMap.begin() ; edgeStart_iter != edgesMap.end() ; ++edgeStart_iter){
				for(std::map<int, edge_info_t*>::iterator edgeEnd_iter = edgeStart_iter->second.begin() ; edgeEnd_iter != edgeStart_iter->second.end() ; ++edgeEnd_iter){
					g->add_edge(edgeStart_iter->first, edgeEnd_iter->first, edgeEnd_iter->second);
				}
			}
			result = true;
		}
		break;
	case grcode_user_gentext:
		{
			result = true;
		}
		break;
	case grcode_user_text:
		{
			mutable_graph_t* g = va_arg(va, mutable_graph_t*);
			int node = va_arg(va, int);
			const char **text = va_arg(va, const char**);
			bgcolor_t* bgcolor = va_arg(va, bgcolor_t*);
			*text = graph_text[node].c_str();
			if(bgcolor != NULL){
				*bgcolor = DEFCOLOR;
			}
			result = true;
			qnotused(g);
		}
		break;
	}
	return result;
}

void freeAllocatedResources(){
	if(form != NULL){
		close_tform(form, 0);
		//form = NULL;
	}
	for(std::map<int, std::map<int, edge_info_t*>>::iterator edgeStart_iter = edgesMap.begin() ; edgeStart_iter != edgesMap.end() ; ++edgeStart_iter){
		for(std::map<int, edge_info_t*>::iterator edgeEnd_iter = edgeStart_iter->second.begin() ; edgeEnd_iter != edgeStart_iter->second.end() ; ++edgeEnd_iter){
			delete edgeEnd_iter->second;
			edgeEnd_iter->second = NULL;
		}
	}
	edgesMap.clear();
}
