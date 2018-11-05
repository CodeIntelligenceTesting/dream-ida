////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "PointerExp.h"
#include "../../Shared/ExpressionUtilities.h"

#include "../../TypeAnalysis/Types/SimpleType/PointerType.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../TypeAnalysis/Types/SimpleType/IntegralTypes.h"
#include "../../TypeAnalysis/Types/TopType.h"

#include "LocalVariable.h"
#include "PointerExp.h"

PointerExp::PointerExp(ExpressionPtr _addressExpression) : Expression(POINTER, NO_SUBSCRIPT)
{
	addressExpression = _addressExpression;
	hasName = false;
	isSimpleExpression = false;
}


PointerExp::~PointerExp(void)
{
}


std::string PointerExp::getExpressionString(){
	std::string subscriptString;
	if(subscript != NO_SUBSCRIPT){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
	}
	return "*(" + addressExpression->getExpressionString() + ")" +  subscriptString;
	//return "mem[" + addressExpression->getExpressionString() + "]" +  subscriptString;
}


char* PointerExp::getColoredExpressionString(){
	char* headCOLSTR = getColoredString("*(", CODE_COLOR);
	//char* headCOLSTR = getColoredString("mem[", CODE_COLOR);
	char* addressExpressionCOLSTR = addressExpression->getColoredExpressionString();
	std::string subscriptString;
	if(subscript != -1){
		subscriptString = "_" + boost::lexical_cast<std::string>(subscript);
	}
	char* tailCOLSTR = getColoredString(")" +  subscriptString, CODE_COLOR);
	//char* tailCOLSTR = getColoredString("]" +  subscriptString, CODE_COLOR);

	int headCOLSTRSize = strlen(headCOLSTR);
	int addressExpressionCOLSTRSize = strlen(addressExpressionCOLSTR);
	int tailCOLSTRSize = strlen(tailCOLSTR);

	char* expCOLSTR = (char*)malloc(headCOLSTRSize + addressExpressionCOLSTRSize + tailCOLSTRSize + 1);
	memcpy(expCOLSTR, headCOLSTR, headCOLSTRSize);
	
	int currentPosition = headCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, addressExpressionCOLSTR, addressExpressionCOLSTRSize);

	currentPosition += addressExpressionCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, tailCOLSTR, tailCOLSTRSize);

	currentPosition += tailCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);

	free(headCOLSTR);
	free(addressExpressionCOLSTR);
	free(tailCOLSTR);

	return expCOLSTR;
}

std::string PointerExp::getExpressionCOLSTR(){
	std::string coloredString;
	//coloredString.append("mem[");
	coloredString.append("*(");
	coloredString.append(addressExpression->getExpressionCOLSTR());
	coloredString.append(")");
	//coloredString.append("]");
	if(subscript != NO_SUBSCRIPT){
		coloredString.append("_" + boost::lexical_cast<std::string>(subscript));
	}
	return coloredString;// + ":" + boost::lexical_cast<std::string>(size_in_bytes);
}

std::string PointerExp::to_json(){
	return "{\"expression_type\":\"PointerExp\", \"size_in_bytes\":\"" +
			boost::lexical_cast<std::string>(size_in_bytes) +
			"\", \"addressExpression\":" + addressExpression->to_json() + "}";
	//return "{\"expression_type\":\"PointerExp\", \"addressExpression\":" + addressExpression->to_json() + "}";
}

std::string PointerExp::getName(){
	if(hasName){
		//return "mem[" + addressExpression->getExpressionString() + "]";
		return "*(" + addressExpression->getExpressionString() + ")";
	}
	else{
		return "";
	}
}


//std::vector<Expression*>* PointerExp::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	if(includePointers){
//		expElements->push_back(this);
//	}
//	std::vector<Expression*>* addressExpressionElements = addressExpression->getExpressionElements(includePointers);
//	expElements->insert(expElements->end(), addressExpressionElements->begin() , addressExpressionElements->end());
//	delete addressExpressionElements;
//	return expElements;
//}

void PointerExp::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	addRootElement(addressExpression, includePointers, elements_out);
	addressExpression->getExpressionElements(includePointers, elements_out);
}


ExpressionPtr PointerExp::deepcopy(){
	//msg("ORIG->addressExpression->expressionType: %s:%s\n", getExpressionString().c_str(), addressExpression->expressionType->getTypeCOLSTR().c_str());
	PointerExpPtr copyPtr = std::make_shared<PointerExp>(addressExpression->deepcopy());
	copyPtr->subscript = subscript;
	copyPtr->hasName = hasName;
	copyPtr->phiSourceBlockId = phiSourceBlockId;
	copyPtr->expressionType = expressionType->deepcopy();
	copyPtr->size_in_bytes = size_in_bytes;
	//copyPtr->set_expression_size(size_in_bytes);
	//msg("copyPtr->addressExpression->expressionType: %s:%s\n", copyPtr->getExpressionString().c_str(), copyPtr->addressExpression->expressionType->getTypeCOLSTR().c_str());
	return copyPtr;
}


//void PointerExp::replaceChildExpression(std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression){
//	if(!applyLocalSimplifications(&addressExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
//		if(nameToBeReplaced.compare(addressExpression->getName()) == 0 && subscriptToBeReplaced == addressExpression->subscript){
//			delete addressExpression;
//			addressExpression = replacingExpression->deepcopy();
//		}
//		else{
//			addressExpression->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
//		}
//	}
//}

void PointerExp::replaceChildExpression(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	if(!applyLocalSimplifications(addressExpression, nameToBeReplaced, subscriptToBeReplaced, replacingExpression)){
		if(addressExpression->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			addressExpression = replacingExpression->deepcopy();
		}
		else{
			addressExpression->replaceChildExpression(nameToBeReplaced, subscriptToBeReplaced, replacingExpression);
		}
	}
}

//bool PointerExp::replaceChildExpressionNoSimplification(std::string nameToBeReplaced, int subscriptToBeReplaced, Expression* replacingExpression){
//	return replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, &addressExpression);
//}

bool PointerExp::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	return replaceOperandNoSimplification(nameToBeReplaced, subscriptToBeReplaced, replacingExpression, addressExpression);
}

ExpressionPtr PointerExp::simplifyExpression(){
	replaceBySimplifiedExpression(addressExpression);
	return std::shared_ptr<Expression>();
}

void PointerExp::updateExpressionType(TypePtr type, TypeMapPtr typeMap){
	PointerTypePtr addressExpressionType = std::make_shared<PointerType>();
	addressExpressionType->pointedToType = type;
	addressExpression->updateExpressionType(addressExpressionType, typeMap);
}

void PointerExp::set_expression_size(int byte_size){
	size_in_bytes = byte_size;
	if(size_in_bytes == 1){
		expressionType = std::make_shared<IntegralType>(CHAR_TYPE);
		PointerTypePtr addressType = std::make_shared<PointerType>();
		addressType->pointedToType = std::make_shared<IntegralType>(CHAR_TYPE);
		addressExpression->expressionType = addressType;
	}
	else if(size_in_bytes == 2){
		expressionType = std::make_shared<IntegralType>(CHAR_TYPE);
	}
}
