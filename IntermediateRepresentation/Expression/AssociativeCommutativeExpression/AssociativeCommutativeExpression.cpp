////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "AssociativeCommutativeExpression.h"
#include "../Constant/NumericConstant.h"
#include "../../../Shared/houseKeepingUtilities.h"
#include "../../../Shared/ExpressionUtilities.h"
//#include "../BinaryNonCommutativeExpression.h"
//#include "../UnaryExpression.h"


AssociativeCommutativeExpression::AssociativeCommutativeExpression(ExpressionVectorPtr _operands, ExpressionType _expressionType) : Expression(_expressionType, NO_SUBSCRIPT)
{
	operands = _operands;
	isSimpleExpression = false;
}

AssociativeCommutativeExpression::~AssociativeCommutativeExpression(void)
{
	//deleteVectorPointer(operands); operands = NULL;
}

std::string AssociativeCommutativeExpression::getExpressionString(){
	std::string expString = "(";
	std::string operationPlusSpaces = " " + getOperationString() + " ";
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		expString += (*op_iter)->getExpressionString() + operationPlusSpaces;
	}
	return expString.substr(0, expString.size() - operationPlusSpaces.size()) + ")";
}

char* AssociativeCommutativeExpression::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	char* operationCOLSTR = getColoredString(" " + getOperationString() + " ", CODE_COLOR);

	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);
	int operationCOLSTRSize = strlen(operationCOLSTR);

	char** operandsCOLSTR = new char*[operands->size()];
	int* operandsCOLSTRSize = new int[operands->size()];

	int expCOLSTRSize = firstBracketCOLSTRSize + secondBracketCOLSTRSize + operationCOLSTRSize * (operands->size() - 1);

	int currentPosition = 0;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		operandsCOLSTR[currentPosition] = (*op_iter)->getColoredExpressionString();
		operandsCOLSTRSize[currentPosition] = strlen(operandsCOLSTR[currentPosition]);
		expCOLSTRSize += operandsCOLSTRSize[currentPosition];
		currentPosition++;
	}

	char* expCOLSTR = (char*)malloc(expCOLSTRSize + 1);
	memcpy(expCOLSTR, firstBracketCOLSTR, firstBracketCOLSTRSize);
	currentPosition = firstBracketCOLSTRSize;
	for(unsigned int i = 0 ; i < operands->size() ; i++){
		memcpy(expCOLSTR + currentPosition, operandsCOLSTR[i], operandsCOLSTRSize[i]);
		currentPosition += operandsCOLSTRSize[i];

		if(i != operands->size() - 1){
			memcpy(expCOLSTR + currentPosition, operationCOLSTR, operationCOLSTRSize);
			currentPosition += operationCOLSTRSize;
		}

		free(operandsCOLSTR[i]);
	}
	memcpy(expCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);
	currentPosition += secondBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);

	free(firstBracketCOLSTR);
	free(secondBracketCOLSTR);
	free(operationCOLSTR);
	delete[] operandsCOLSTR;
	delete[] operandsCOLSTRSize;

	return expCOLSTR;
}

std::string AssociativeCommutativeExpression::getExpressionCOLSTR(){
	std::string coloredString = "(";
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		coloredString.append((*op_iter)->getExpressionCOLSTR());
		if(op_iter + 1 != operands->end())
			coloredString.append(" " + getOperationString() + " ");
	}
	coloredString.append(")");
	return coloredString;
}

void AssociativeCommutativeExpression::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		ExpressionPtr operandPtr = *op_iter;
		createExpressionElements(operandPtr, includePointers, elements_out);
	}
}

void AssociativeCommutativeExpression::replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	//msg("replacingExpression(%x)\n", replacingExpression);
	//msg("handling %s\n", getExpressionString().c_str());
	//msg("name(%s), subscript(%d), replacingExpression(%s)\n", nameToBeReplaced.c_str(), subscriptToBeReplaced, replacingExpression !=NULL ? replacingExpression->getExpressionString().c_str():"NULL");
	std::vector<ExpressionPtr> addedOperands;
	std::vector<ExpressionPtr>::iterator op_iter = operands->begin();
	while(op_iter != operands->end()){
		//msg("Associative::replaceChildExpression(%s, %s, %d, %s)\n", (*op_iter)->getExpressionString().c_str()
		//	, nameToBeReplaced.c_str(), subscriptToBeReplaced, replacingExpression->getExpressionString().c_str());
		//ExpressionPtr operandExp = *op_iter;
		if(applyLocalSimplifications(*op_iter, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
			//msg("applyLocalSimplifications\n");
			ExpressionPtr operandExp = *op_iter;
			if(operandExp->type == type){
				AssociativeCommutativeExpressionPtr operandAssCommExp = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(operandExp);
				//if(operation.compare(operandAssCommExp->operation) == 0){
				ExpressionVectorPtr subOperands = operandAssCommExp->operands;
				for(std::vector<ExpressionPtr>::iterator subOp_iter = subOperands->begin() ; subOp_iter != subOperands->end() ; ++subOp_iter){
					addedOperands.push_back((*subOp_iter)->deepcopy());
				}
				op_iter = operands->erase(op_iter);
				//}
				//else{
				//	op_iter++;
				//}
			}
			else{
				op_iter++;
			}
		}
		else{
			//msg("dontApplyLocalSimplifications\n");
			ExpressionPtr operandExp = *op_iter;
			if(operandExp->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			
				if(replacingExpression->type == type){
					AssociativeCommutativeExpressionPtr replacingAssComExp = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(replacingExpression);
					
					//if(operation.compare(replacingAssComExp->operation) == 0){
						op_iter = operands->erase(op_iter);
						for(std::vector<ExpressionPtr>::iterator replacing_op_iter = replacingAssComExp->operands->begin() ; replacing_op_iter != replacingAssComExp->operands->end() ; ++replacing_op_iter){
							addedOperands.push_back((*replacing_op_iter)->deepcopy());
						}
					//}
					//else{
					//	delete operandExp;
					//	*op_iter = replacingExpression->deepcopy();
					//	op_iter++;
					//}
				}
				else{
					*op_iter = replacingExpression->deepcopy();
					op_iter++;
				}
			}
			else{
				operandExp->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
				op_iter++;
			}
		}
	}
	operands->insert(operands->end(), addedOperands.begin(), addedOperands.end());
	//simplifyExpression();
}

bool AssociativeCommutativeExpression::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	bool result = false;
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		result |= replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, *op_iter);
	}
	return result;
}

void AssociativeCommutativeExpression::simplifyOperands(){
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end(); ++op_iter){
		ExpressionPtr operandExp = *op_iter;
		if(!operandExp->isSimpleExpression){
			replaceBySimplifiedExpression(*op_iter);
		}
	}
}

bool AssociativeCommutativeExpression::equals(const ExpressionPtr& toThisExpression){
	if(type == toThisExpression->type){
		AssociativeCommutativeExpressionPtr toThisAssociativeCommutativeExpression = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(toThisExpression);
		//if(operation.compare(toThisAssociativeCommutativeExpression->operation) == 0){
		std::vector<ExpressionPtr> otherOperandsCopy;
		otherOperandsCopy.insert(otherOperandsCopy.end(), toThisAssociativeCommutativeExpression->operands->begin(), toThisAssociativeCommutativeExpression->operands->end());
		for(std::vector<ExpressionPtr>::iterator thisOp_iter = operands->begin() ; thisOp_iter != operands->end() ; ++thisOp_iter){
			ExpressionPtr thisOperand = *thisOp_iter;
			std::vector<ExpressionPtr>::iterator otherOp_iter = otherOperandsCopy.begin();
			bool foundEqualOperand = false;
			while(otherOp_iter != otherOperandsCopy.end()){
				ExpressionPtr otherOperand = *otherOp_iter;
				if(thisOperand->equals(otherOperand)){
					otherOp_iter = otherOperandsCopy.erase(otherOp_iter);
					foundEqualOperand = true;
					break;
				}
				else{
					++otherOp_iter;
				}
			}
			if(!foundEqualOperand){
				return false;
			}
		}
		if(otherOperandsCopy.size() == 0){
			return true;
		}
		//}
	}
	return false;
}

bool AssociativeCommutativeExpression::equalsDiscardConstants(const AssociativeCommutativeExpressionPtr& firstExpression, const AssociativeCommutativeExpressionPtr& secondExpression){
	//if(operation.compare("+") == 0){
	//	if(firstExpression->operation.compare("*") == 0 && secondExpression->operation.compare("*") == 0){
	if(firstExpression->type == secondExpression->type){
		AssociativeCommutativeExpressionPtr firstExpressionCopy = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(firstExpression->deepcopy());
		AssociativeCommutativeExpressionPtr secondExpressionCopy = std::dynamic_pointer_cast<AssociativeCommutativeExpression>(secondExpression->deepcopy());
		firstExpressionCopy->removeConstantOperands();
		secondExpressionCopy->removeConstantOperands();
		bool result = firstExpressionCopy->equals(secondExpressionCopy);
		return result;
	}
	//}
	return false;
}

void AssociativeCommutativeExpression::removeConstantOperands(){
	std::vector<ExpressionPtr>::iterator op_iter = operands->begin();
	while(op_iter != operands->end()){
		op_iter = ((*op_iter)->type == NUMERIC_CONSTANT) ? operands->erase(op_iter) : op_iter + 1;
		//if((*op_iter)->type == NUMERIC_LITERAL){
		//	if(!((NumericLiteral*)*op_iter)->isSymbolicValue) // Need better handling: think about simplifying symbolic constants
		//		op_iter = operands->erase(op_iter);
		//}
		//else{
		//	++op_iter;
		//}
	}
}

bool AssociativeCommutativeExpression::doesHavePointerOperand(TypeMapPtr typeMap){
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		if((*op_iter)->getExpresstionType(typeMap)->isIntegerType())
			return true;
	}
	return false;
}

std::string AssociativeCommutativeExpression::to_json(){
	std::string expression_type;
	if(getOperationString().compare("+") == 0)
		expression_type = "AdditionExpression";
	else if(getOperationString().compare("*") == 0)
		expression_type = "MultiplicationExpression";
	else if(getOperationString().compare("|") == 0 || getOperationString().compare("||") == 0)
		expression_type = "ORExpression";
	else if(getOperationString().compare("&") == 0 || getOperationString().compare("&&") == 0)
		expression_type = "ANDExpression";
	else if(getOperationString().compare("V") == 0)
		expression_type = "XORExpression";
	std::string json_string = "{\"expression_type\":\"" + expression_type + "\", \"operands\":[";
	for(std::vector<ExpressionPtr>::iterator op_iter = operands->begin() ; op_iter != operands->end() ; ++op_iter){
		json_string.append((*op_iter)->to_json() + ", ");
	}
	return json_string.substr(0, json_string.length() - 2) + "]}";
}
