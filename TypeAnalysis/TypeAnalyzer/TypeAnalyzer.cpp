////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "TypeAnalyzer.h"

#include "IntermediateRepresentation/Instruction/Instruction.h"
#include "IntermediateRepresentation/Instruction/Phi_Function.h"
#include "IntermediateRepresentation/Instruction/BinaryInstruction/Assignment.h"
#include "IntermediateRepresentation/Expression/Constant/NumericConstant.h"
#include "IntermediateRepresentation/Expression/AssociativeCommutativeExpression/AdditionExpression.h"
#include "IntermediateRepresentation/Expression/UnaryExpression/AddressExpression.h"
#include "TypeAnalysis/Types/SimpleType/IntegralTypes.h"
#include "TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "TypeAnalysis/Types/SimpleType/PointerType.h"
#include "TypeAnalysis/Types/TopType.h"

TypeAnalyzer::TypeAnalyzer(definitionsMapPtr _definitionsMap, usesMapPtr _usesMap)
{
	definitionsMap = _definitionsMap;
	usesMap = _usesMap;
	typeMap = std::make_shared<TypeMap>();
	//constructTypeMap();
	tmp=1;
	//unifyUsesAndDefinitionsTypes();
}


TypeAnalyzer::~TypeAnalyzer(void)
{
}


void TypeAnalyzer::analyzeTypes(){
	int i = 0;
	constructTypeMap();
	typeMap->newTypesResolved = true;
	while(i<=4 && typeMap->shouldPerformNewAnalysisRound()){
		typeMap->startNewAnalysisRound();

		for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
			for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
				InstructionPtr definingInstruction = subscript_iter->second;
				//msg("def: %s_%d\n", name_iter->first.c_str() , subscript_iter->first);
				if(definingInstruction.get() != NULL){
					ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(name_iter->first, subscript_iter->first);
					if(definedExpression.get() != NULL){//handle related register names
						if(definedExpression->getExpresstionType(typeMap)->type == TOP_TYPE){
							//msg("Type1(%s) = void\n", definedExpression->getExpressionString().c_str());
							setDefTypeFromDefiningInstruction(definingInstruction);
						}
						if(definedExpression->getExpresstionType(typeMap)->type != TOP_TYPE){
							//msg("Type2(%s) = %s\n", definedExpression->getExpressionString().c_str(), definedExpression->expressionType->getTypeCOLSTR().c_str());
							propagateTypeToDefiningInstruction(definedExpression, definingInstruction);
							propagateTypeToUsingInstructions(definedExpression);
						}
					}
				}
			}
		}
		i++;
	}


	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
		for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
			InstructionPtr definingInstruction = subscript_iter->second;
			if(definingInstruction.get() != NULL){
				ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(name_iter->first, subscript_iter->first);
				if(definedExpression.get() != NULL){//handle related register names
					definedExpression->expressionType = typeMap->getType(name_iter->first, subscript_iter->first);
				}
			}
		}
	}

	//for(std::map<std::string, std::map<int, TypePtr>>::iterator name_iter = typeMap->types.begin() ; name_iter != typeMap->types.end() ; ++name_iter){
	//	for(std::map<int, TypePtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
	//		TypePtr type = typeMap->getType(name_iter->first, subscript_iter->first);
	//		//InstructionPtr definingInstruction = subscript_iter->second;
	//		//	if(definingInstruction.get() != NULL){
	//		//		ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(name_iter->first, subscript_iter->first);
	//		//		if(definedExpression.get() != NULL){//handle related register names
	//		//			definedExpression->expressionType = type;
	//		//		}
	//		//	}
	//		msg("#####TYPE_%d(%s_%d) = %s\n",tmp, name_iter->first.c_str(), subscript_iter->first, subscript_iter->second->getTypeCOLSTR().c_str());
	//	}
	//}
	//msg("DONE");
	tmp++;
}

//void TypeAnalyzer::unifyUsesAndDefinitionsTypes(){
//	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->begin() ; name_iter != definitionsMap->end() ; ++name_iter){
//		for(std::map<int, InstructionPtr>::iterator subscript_iter = name_iter->second.begin() ; subscript_iter != name_iter->second.end() ; ++subscript_iter){
//			InstructionPtr definingInstruction = subscript_iter->second;
//			if(definingInstruction.get() != NULL){
//				ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(name_iter->first, subscript_iter->first);
//				if(definedExpression.get() != NULL){//handle related register names
//					if(definedExpression->expressionType->type != TOP_TYPE){
//						updateUsesType(definedExpression, definedExpression->expressionType);
//					}
//				}
//			}
//		}
//	}
//}

void TypeAnalyzer::propagateTypeToDefiningInstruction(ExpressionPtr definedExpression, InstructionPtr definingInstruction){
	TypePtr definitionType = definedExpression->getExpresstionType(typeMap);
	if(definingInstruction->type == ASSIGNMENT){
		ExpressionPtr rhsOperand = (std::dynamic_pointer_cast<Assignment>(definingInstruction))->rhsOperand;
		updateExpressionType(rhsOperand, definitionType);
		/*if(rhsOperand->isSimpleExpression){
			
		}
		else if(rhsOperand->type == POINTER){

		}
		else if(rhsOperand->type == ADDITION_EXPRESSION){
			
		}*/
	}
	else if(definingInstruction->type == PHI_FUNCTION){
		Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(definingInstruction);
		propagateTypeToPhiFunction(phiFunction, definitionType);
	}
}

void TypeAnalyzer::propagateTypeToUsingInstructions(ExpressionPtr usedExpression){
	//msg("propagateTypeToUsingInstructions(%s)\n", usedExpression->getExpressionString().c_str());
	std::vector<InstructionPtr>* usingInstructions = &(usesMap->operator[](usedExpression->getName()).operator[](usedExpression->subscript));
	for(std::vector<InstructionPtr>::iterator use_iter = usingInstructions->begin() ; use_iter != usingInstructions->end() ; ++use_iter){
		InstructionPtr usingInstruction = *use_iter;
		if(usingInstruction->type == ASSIGNMENT){
			AssignmentPtr usingAssignment = std::dynamic_pointer_cast<Assignment>(usingInstruction);
			if(usingAssignment->lhsOperand->getExpresstionType(typeMap)->isVoidType()){
				updateExpressionType(usingAssignment->lhsOperand, usingAssignment->rhsOperand->getExpresstionType(typeMap));
			}
		}
		else if(usingInstruction->type == PHI_FUNCTION){
			Phi_FunctionPtr phiFunction = std::dynamic_pointer_cast<Phi_Function>(usingInstruction);
			propagateTypeToPhiFunction(phiFunction, usedExpression->getExpresstionType(typeMap));
		}
	}
}

void TypeAnalyzer::propagateTypeToPhiFunction(Phi_FunctionPtr phiFunction, TypePtr type){
	return;
	if(phiFunction->target->getExpresstionType(typeMap)->isVoidType())
		updateExpressionType(phiFunction->target, type);
	for(std::vector<ExpressionPtr>::iterator arg_iter = phiFunction->arguments->begin() ; arg_iter != phiFunction->arguments->end() ; ++arg_iter){
		ExpressionPtr phiArgument = *arg_iter;
		if(phiArgument->getExpresstionType(typeMap)->isVoidType())
			updateExpressionType(phiArgument, type);
	}
}

void TypeAnalyzer::analyzeTypesInAssignment(AssignmentPtr assignment){
	//TODO split into two functions
	/*if(assignment->rhsOperand->type == ADDITION_EXPRESSION){
		msg("Addition: %s\n", assignment->getInstructionString().c_str());
		ExpressionVectorPtr ops = std::dynamic_pointer_cast<AdditionExpression>(assignment->rhsOperand)->operands;
		for(std::vector<ExpressionPtr>::iterator it = ops->begin() ; it != ops->end() ; ++it){
			msg("\t%s %s\n", (*it)->getExpressionString().c_str(), (*it)->getExpresstionType()->getTypeCOLSTR().c_str());
		}
	}*/
	if(assignment->lhsOperand->getExpresstionType(typeMap)->type == TOP_TYPE){
		updateExpressionType(assignment->lhsOperand, assignment->rhsOperand->getExpresstionType(typeMap));
	}
	else{
		
	}
}

void TypeAnalyzer::setDefTypeFromDefiningInstruction(InstructionPtr definingInstruction){
	if(definingInstruction->type == ASSIGNMENT)
		analyzeTypesInAssignment(std::dynamic_pointer_cast<Assignment>(definingInstruction));
	else if(definingInstruction->type == PHI_FUNCTION){
		//unifytypes
	}
}

TypePtr TypeAnalyzer::getTypeFromAddressExpression(AddressExpressionPtr addressExpression){
  return std::make_shared<PointerType>();
}

TypePtr TypeAnalyzer::getTypeFromAdditionExpression(AdditionExpressionPtr additionExpression){
	ExpressionVectorPtr operands = additionExpression->operands;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		ExpressionPtr operand = *op_iter;
		
	}
	return std::shared_ptr<TopType>();
}

void TypeAnalyzer::updateExpressionType(ExpressionPtr expression, TypePtr type){
	//msg("%s->updateExpressionType(%s)\n", expression->getExpressionString().c_str(), type->getTypeCOLSTR().c_str());
	expression->updateExpressionType(type, typeMap);
	/*typeMap->setType(expression->getName(), expression->subscript, type);
	if(isDefined(expression)){
		updateDefinitionType(expression, type);
	}*/
	//updateUsesType(expression, type);
}

//void TypeAnalyzer::updateDefinitionType(ExpressionPtr expression, TypePtr type){
//	InstructionPtr definingInstruction = definitionsMap->operator[](expression->getName()).operator[](expression->subscript);
//	ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(expression->getName(), expression->subscript);
//	definedExpression->expressionType = type;
//}

//void TypeAnalyzer::updateUsesType(ExpressionPtr expression, TypePtr type){
//	std::vector<ExpressionPtr> uses;
//	getExpressionUses(expression, uses);
//	setExpressionsType(uses, type);
//}

bool TypeAnalyzer::isDefined(ExpressionPtr exp){
	std::map<std::string, std::map<int, InstructionPtr>>::iterator name_iter = definitionsMap->find(exp->getName());
	if(name_iter != definitionsMap->end())
		return name_iter->second.find(exp->subscript) != name_iter->second.end();
	return false;
}

//bool TypeAnalyzer::isUsed(ExpressionPtr exp){
//	std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator name_iter = usesMap->find(exp->getName());
//	if(name_iter != usesMap->end())
//		return name_iter->second.find(exp->subscript) != name_iter->second.end();
//	return false;
//}

void TypeAnalyzer::getExpressionUses(ExpressionPtr exp, std::vector<ExpressionPtr>& uses){
	std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator name_iter = usesMap->find(exp->getName());
	if(name_iter != usesMap->end()){
		std::map<int, std::vector<InstructionPtr>>::iterator subscript_iter = name_iter->second.find(exp->subscript);
		if(subscript_iter != name_iter->second.end()){
			for(std::vector<InstructionPtr>::iterator use_inst_iter = subscript_iter->second.begin() ; use_inst_iter != subscript_iter->second.end() ; ++use_inst_iter){
				(*use_inst_iter)->getUsesOfExpression(name_iter->first, subscript_iter->first, uses);
			}
		}
	}
}

bool TypeAnalyzer::isNewTypeMoreSpecific(TypePtr currentType, TypePtr newType){
	if(currentType->type == TOP_TYPE){
		return newType->type != TOP_TYPE;
	}
	else if(currentType->type == POINTER_TYPE && newType->type == POINTER_TYPE){
		PointerTypePtr currentPointerType = std::dynamic_pointer_cast<PointerType>(currentType);
		PointerTypePtr newPointerType = std::dynamic_pointer_cast<PointerType>(newType);
		return isNewTypeMoreSpecific(currentPointerType->pointedToType, newPointerType->pointedToType);
	}
	return false;
}

void TypeAnalyzer::setExpressionsType(std::vector<ExpressionPtr>& expressions, TypePtr type){
	for(std::vector<ExpressionPtr>::iterator exp_iter = expressions.begin() ; exp_iter != expressions.end() ; ++exp_iter){
		(*exp_iter)->expressionType = type;
	}
}

void TypeAnalyzer::constructTypeMap(){
	for(std::map<std::string, std::map<int, InstructionPtr>>::iterator def_name_iter = definitionsMap->begin() ; def_name_iter != definitionsMap->end() ; ++def_name_iter){
		std::string def_name = def_name_iter->first;
		for(std::map<int, InstructionPtr>::iterator def_subscript_iter = def_name_iter->second.begin() ; def_subscript_iter != def_name_iter->second.end() ; ++def_subscript_iter){
			int def_subscript = def_subscript_iter->first;
			InstructionPtr definingInstruction = def_subscript_iter->second;
			ExpressionPtr definedExpression = definingInstruction->getDefinedExpression(def_name, def_subscript);
			if(definedExpression.get() != NULL){
				TypePtr type = definedExpression->expressionType;
				if(type->type == TOP_TYPE)
					type = getTypeFromUses(def_name, def_subscript);
				typeMap->setType(def_name, def_subscript, type);
				definedExpression->expressionType = type;
				//msg("%s (%s)\n", definedExpression->getExpressionString().c_str(), typeMap->getType(def_name, def_subscript)->getTypeCOLSTR().c_str());
			}
		}
	}
}

TypePtr TypeAnalyzer::getTypeFromUses(const std::string& name, int subscript){
	std::map<std::string, std::map<int, std::vector<InstructionPtr>>>::iterator use_name_iter = usesMap->find(name);
	if(use_name_iter != usesMap->end()){
		std::map<int, std::vector<InstructionPtr>>::iterator use_subscript_iter = use_name_iter->second.find(subscript);
		if(use_subscript_iter != use_name_iter->second.end()){
			for(std::vector<InstructionPtr>::iterator use_inst_iter = use_subscript_iter->second.begin() ; use_inst_iter != use_subscript_iter->second.end() ; ++use_inst_iter){
				std::vector<ExpressionPtr> uses;
				(*use_inst_iter)->getUsesOfExpression(name, subscript, uses);
				for(std::vector<ExpressionPtr>::iterator use_iter = uses.begin() ; use_iter != uses.end() ; ++use_iter){
					TypePtr useType = (*use_iter)->expressionType;
					//msg("\t%s\n", (*use_iter)->getInstructionString().c_str());
					if(useType->type != TOP_TYPE)
						return useType;
					//msg("\t\t%s\n", useType->getTypeCOLSTR().c_str());
				}
			}
		}
	}
	return std::make_shared<TopType>();
}
