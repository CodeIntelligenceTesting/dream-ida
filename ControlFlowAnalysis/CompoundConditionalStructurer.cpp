////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "CompoundConditionalStructurer.h"
#include "../IntermediateRepresentation/ControlFlowGraph.h"
#include "GraphTraversal/GraphTraversal.h"
#include "../Shared/ExpressionUtilities.h"
#include "GraphUtilities/GraphConverter.h"

//TODO delete
#include <idp.hpp>

CompoundConditionalStructurer::CompoundConditionalStructurer(ControlFlowGraphPtr _controlFlowGraph)
{
	controlFlowGraph = _controlFlowGraph;
}


CompoundConditionalStructurer::~CompoundConditionalStructurer(void)
{
}

void CompoundConditionalStructurer::structureCompoundConditionals(){
	bool structuringDone = false;
	while(!structuringDone){
		structuringDone = true;
		intVectorPtr postorder = getPostOrder();
		for(std::vector<int>::iterator post_it = postorder->begin() ; post_it != postorder->end() ; ++post_it){
			if(*post_it < controlFlowGraph->nodes->size()){
				NodePtr currentNode = controlFlowGraph->nodes->at(*post_it);
				if(isHeadOf_A_AND_B(currentNode)){
					msg("isHeadOf_A_AND_B(%d)\n", currentNode->id);
					merge_A_AND_B(currentNode);
					structuringDone = false;
				}
				else if(isHeadOf_NOT_A_OR_B(currentNode)){
					msg("isHeadOf_NOT_A_OR_B(%d)\n", currentNode->id);
					merge_NOT_A_OR_B(currentNode);
					structuringDone = false;
				}
				else if(isHeadOf_A_OR_B(currentNode)){
					msg("isHeadOf_A_OR_B(%d)\n", currentNode->id);
					merge_A_OR_B(currentNode);
					structuringDone = false;
				}
				else if(isHeadOf_NOT_A_AND_B(currentNode)){
					msg("isHeadOf_NOT_A_AND_B(%d)\n", currentNode->id);
					merge_NOT_A_AND_B(currentNode);
					structuringDone = false;
				}
			}
		}
	}
}

intVectorPtr CompoundConditionalStructurer::getPostOrder(){
	GraphConverter graphConverter;
	Graph cfg;
	graphConverter.convertToBoostGraph(controlFlowGraph, cfg);
	GraphTraversal graphTraversal(cfg);
	//graphTraversal.applyDepthFirstSearch();
	return graphTraversal.postorder;
}


bool CompoundConditionalStructurer::isHeadOf_A_AND_B(const NodePtr &head){
	if(head->nodeType == TWO_WAY_NODE){
		TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
		NodePtr trueSucc = controlFlowGraph->nodes->at(twoWayHead->trueSuccessorID);
		if(trueSucc->nodeType == TWO_WAY_NODE && trueSucc->instructions->size() == 1 && controlFlowGraph->getPredecessors(trueSucc->id)->size() == 1){
			TwoWayNodePtr twoWayTrueSucc = std::dynamic_pointer_cast<TwoWayNode>(trueSucc);
			return twoWayHead->falseSuccessorID == twoWayTrueSucc->falseSuccessorID;
		}
	}
	return false;
}

bool CompoundConditionalStructurer::isHeadOf_NOT_A_OR_B(const NodePtr &head){
	if(head->nodeType == TWO_WAY_NODE){
		TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
		NodePtr trueSucc = controlFlowGraph->nodes->at(twoWayHead->trueSuccessorID);
		if(trueSucc->nodeType == TWO_WAY_NODE && trueSucc->instructions->size() == 1 && controlFlowGraph->getPredecessors(trueSucc->id)->size() == 1){
			msg("trueSucc:\n");
			for(std::vector<InstructionPtr>::iterator it = trueSucc->instructions->begin() ; it != trueSucc->instructions->end() ; ++it)
				msg("\t%s\n", (*it)->getInstructionString().c_str());
			TwoWayNodePtr twoWayTrueSucc = std::dynamic_pointer_cast<TwoWayNode>(trueSucc);
			return twoWayHead->falseSuccessorID == twoWayTrueSucc->trueSuccessorID;
		}
	}
	return false;
}

bool CompoundConditionalStructurer::isHeadOf_A_OR_B(const NodePtr &head){
	if(head->nodeType == TWO_WAY_NODE){
		TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
		NodePtr falseSucc = controlFlowGraph->nodes->at(twoWayHead->falseSuccessorID);
		if(falseSucc->nodeType == TWO_WAY_NODE && falseSucc->instructions->size() == 1 && controlFlowGraph->getPredecessors(falseSucc->id)->size() == 1){
			TwoWayNodePtr twoWayFalseSucc = std::dynamic_pointer_cast<TwoWayNode>(falseSucc);
			return twoWayHead->trueSuccessorID == twoWayFalseSucc->trueSuccessorID;
		}
	}
	return false;
}

bool CompoundConditionalStructurer::isHeadOf_NOT_A_AND_B(const NodePtr &head){
	if(head->nodeType == TWO_WAY_NODE){
		TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
		NodePtr falseSucc = controlFlowGraph->nodes->at(twoWayHead->falseSuccessorID);
		if(falseSucc->nodeType == TWO_WAY_NODE && falseSucc->instructions->size() == 1 && controlFlowGraph->getPredecessors(falseSucc->id)->size() == 1){
			TwoWayNodePtr twoWayFalseSucc = std::dynamic_pointer_cast<TwoWayNode>(falseSucc);
			return twoWayHead->trueSuccessorID == twoWayFalseSucc->falseSuccessorID;
		}
	}
	return false;
}


void CompoundConditionalStructurer::merge_A_AND_B(NodePtr &head){
	TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
	TwoWayNodePtr twoWayTrueSucc = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(twoWayHead->trueSuccessorID));
	ConditionalJumpPtr conditionalA = std::dynamic_pointer_cast<ConditionalJump>(twoWayHead->instructions->back());
	ConditionalJumpPtr conditionalB = std::dynamic_pointer_cast<ConditionalJump>(twoWayTrueSucc->instructions->back());
	ANDExpressionPtr compoundCondition = getCompundCondition_AND(conditionalA->condition, conditionalB->condition);
	if(!isExpressionPointerNull(compoundCondition))
		conditionalA->condition = compoundCondition;
	conditionalA->target = twoWayTrueSucc->trueSuccessorID;
	twoWayHead->trueSuccessorID = conditionalA->target;
	//nodesEliminator.removeNode(controlFlowGraph, twoWayTrueSucc->id);
	controlFlowGraph->removeNode(twoWayTrueSucc->id);
}

void CompoundConditionalStructurer::merge_NOT_A_OR_B(NodePtr &head){
	TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
	TwoWayNodePtr twoWayTrueSucc = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(twoWayHead->trueSuccessorID));
	ConditionalJumpPtr conditionalA = std::dynamic_pointer_cast<ConditionalJump>(twoWayHead->instructions->back());
	ConditionalJumpPtr conditionalB = std::dynamic_pointer_cast<ConditionalJump>(twoWayTrueSucc->instructions->back());
	ExpressionPtr neg_A = getNegatedExpression(conditionalA->condition);
	ORExpressionPtr compoundCondition = getCompundCondition_OR(neg_A, conditionalB->condition);
	if(!isExpressionPointerNull(compoundCondition))
		conditionalA->condition = compoundCondition;
	conditionalA->target = twoWayTrueSucc->trueSuccessorID;
	twoWayHead->trueSuccessorID = conditionalA->target;
	twoWayHead->falseSuccessorID = twoWayTrueSucc->falseSuccessorID;
	//nodesEliminator.removeNode(controlFlowGraph, twoWayTrueSucc->id);
	controlFlowGraph->removeNode(twoWayTrueSucc->id);
}

void CompoundConditionalStructurer::merge_A_OR_B(NodePtr &head){
	TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
	TwoWayNodePtr twoWayFalseSucc = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(twoWayHead->falseSuccessorID));
	ConditionalJumpPtr conditionalA = std::dynamic_pointer_cast<ConditionalJump>(twoWayHead->instructions->back());
	ConditionalJumpPtr conditionalB = std::dynamic_pointer_cast<ConditionalJump>(twoWayFalseSucc->instructions->back());
	ORExpressionPtr compoundCondition = getCompundCondition_OR(conditionalA->condition, conditionalB->condition);
	if(!isExpressionPointerNull(compoundCondition))
		conditionalA->condition = compoundCondition;
	twoWayHead->falseSuccessorID = twoWayFalseSucc->falseSuccessorID;
	//nodesEliminator.removeNode(controlFlowGraph, twoWayFalseSucc->id);
	controlFlowGraph->removeNode(twoWayFalseSucc->id);
}

void CompoundConditionalStructurer::merge_NOT_A_AND_B(NodePtr &head){
	TwoWayNodePtr twoWayHead = std::dynamic_pointer_cast<TwoWayNode>(head);
	TwoWayNodePtr twoWayFalseSucc = std::dynamic_pointer_cast<TwoWayNode>(controlFlowGraph->nodes->at(twoWayHead->falseSuccessorID));
	ConditionalJumpPtr conditionalA = std::dynamic_pointer_cast<ConditionalJump>(twoWayHead->instructions->back());
	ConditionalJumpPtr conditionalB = std::dynamic_pointer_cast<ConditionalJump>(twoWayFalseSucc->instructions->back());
	ExpressionPtr neg_A = getNegatedExpression(conditionalA->condition);
	ANDExpressionPtr compoundCondition = getCompundCondition_AND(neg_A, conditionalB->condition);
	if(!isExpressionPointerNull(compoundCondition))
		conditionalA->condition = compoundCondition;
	conditionalA->target = twoWayFalseSucc->trueSuccessorID;
	twoWayHead->trueSuccessorID = conditionalA->target;
	twoWayHead->falseSuccessorID = twoWayFalseSucc->falseSuccessorID;
	//nodesEliminator.removeNode(controlFlowGraph, twoWayFalseSucc->id);
	controlFlowGraph->removeNode(twoWayFalseSucc->id);
}


//ExpressionPtr CompoundConditionalStructurer::getNegatedExpression(ExpressionPtr &booleanExpression){
//	switch(booleanExpression->type){
//	case LOGICAL_NOT_EXPRESSION:
//		{
//			return std::dynamic_pointer_cast<LogicalNotExpression>(booleanExpression)->operand;
//		}
//	case LOWLEVEL_CONDITION:
//		{
//			LogicalNotExpressionPtr negatedExpression = std::make_shared<LogicalNotExpression>(booleanExpression, true);
//			return negatedExpression;
//		}
//	case HIGHLEVEL_CONDITION:
//		{
//			HighLevelConditionPtr highLevelCondition = std::dynamic_pointer_cast<HighLevelCondition>(booleanExpression);
//			highLevelCondition->comparisonOperand = negatedConditionalOperationsMap[highLevelCondition->comparisonOperand];
//			return highLevelCondition;
//		}
//	case AND_EXPRESSION:
//		{
//			ExpressionVectorPtr ORoperands = std::make_shared<std::vector<ExpressionPtr>>();
//			ANDExpressionPtr andExpression = std::dynamic_pointer_cast<ANDExpression>(booleanExpression);
//			for(std::vector<ExpressionPtr>::iterator op_it = andExpression->operands->begin() ; op_it != andExpression->operands->end() ; ++op_it){
//				ORoperands->push_back(getNegatedExpression(*op_it));
//			}
//			ORExpressionPtr negatedExpression = std::make_shared<ORExpression>(ORoperands, true);
//			return negatedExpression;
//		}
//	case OR_EXPRESSION:
//		{
//			ExpressionVectorPtr ANDoperands = std::make_shared<std::vector<ExpressionPtr>>();
//			ORExpressionPtr orExpression = std::dynamic_pointer_cast<ORExpression>(booleanExpression);
//			for(std::vector<ExpressionPtr>::iterator op_it = orExpression->operands->begin() ; op_it != orExpression->operands->end() ; ++op_it){
//				ANDoperands->push_back(getNegatedExpression(*op_it));
//			}
//			ANDExpressionPtr negatedExpression = std::make_shared<ANDExpression>(ANDoperands, true);
//			return negatedExpression;
//		}
//	}
//}
//
//ANDExpressionPtr CompoundConditionalStructurer::getCompundCondition_AND(ExpressionPtr &conditionA, ExpressionPtr &conditionB){
//	ANDExpressionPtr result = std::shared_ptr<ANDExpression>();
//	if(conditionA->type != AND_EXPRESSION && conditionB->type != AND_EXPRESSION){
//		ExpressionVectorPtr compoundConditionOperands = std::make_shared<std::vector<ExpressionPtr>>();
//		compoundConditionOperands->push_back(conditionA);
//		compoundConditionOperands->push_back(conditionB);
//		ANDExpressionPtr compoundCondition = std::make_shared<ANDExpression>(compoundConditionOperands, true);
//		result = compoundCondition;
//	}
//	else if(conditionA->type == AND_EXPRESSION){
//		std::dynamic_pointer_cast<ANDExpression>(conditionA)->operands->push_back(conditionB);
//	}
//	else if(conditionB->type == AND_EXPRESSION){
//		ANDExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ANDExpression>(conditionB);
//		compoundConditionB->operands->push_back(conditionA);
//		result = compoundConditionB;
//	}
//	else{
//		ANDExpressionPtr compoundConditionA = std::dynamic_pointer_cast<ANDExpression>(conditionA);
//		ANDExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ANDExpression>(conditionB);
//		for(std::vector<ExpressionPtr>::iterator op_it = compoundConditionB->operands->begin() ; op_it != compoundConditionB->operands->end() ; ++op_it){
//			compoundConditionA->operands->push_back(*op_it);
//		}
//	}
//	return result;
//}
//
//ORExpressionPtr CompoundConditionalStructurer::getCompundCondition_OR(ExpressionPtr &conditionA, ExpressionPtr &conditionB){
//	ORExpressionPtr result = std::shared_ptr<ORExpression>();
//	if(conditionA->type != OR_EXPRESSION && conditionB->type != OR_EXPRESSION){
//		ExpressionVectorPtr compoundConditionOperands = std::make_shared<std::vector<ExpressionPtr>>();
//		compoundConditionOperands->push_back(conditionA);
//		compoundConditionOperands->push_back(conditionB);
//		ORExpressionPtr compoundCondition = std::make_shared<ORExpression>(compoundConditionOperands, true);
//		result = compoundCondition;
//	}
//	else if(conditionA->type == OR_EXPRESSION){
//		std::dynamic_pointer_cast<ORExpression>(conditionA)->operands->push_back(conditionB);
//	}
//	else if(conditionB->type == OR_EXPRESSION){
//		ORExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ORExpression>(conditionB);
//		compoundConditionB->operands->push_back(conditionA);
//		result = compoundConditionB;
//	}
//	else{
//		ORExpressionPtr compoundConditionA = std::dynamic_pointer_cast<ORExpression>(conditionA);
//		ORExpressionPtr compoundConditionB = std::dynamic_pointer_cast<ORExpression>(conditionB);
//		for(std::vector<ExpressionPtr>::iterator op_it = compoundConditionB->operands->begin() ; op_it != compoundConditionB->operands->end() ; ++op_it){
//			compoundConditionA->operands->push_back(*op_it);
//		}
//	}
//	return result;
//}
