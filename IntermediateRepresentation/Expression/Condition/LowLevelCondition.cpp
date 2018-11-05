////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "../Flag.h"
#include "LowLevelCondition.h"

//#include "../../../Shared/houseKeepingUtilities.h"

LowLevelCondition::LowLevelCondition(FlagVectorPtr _testedFlags, BranchType _branchType) : Condition(LOWLEVEL_CONDITION, NO_SUBSCRIPT)
{
	testedFlags = _testedFlags;
	branchType = _branchType;
	isSimpleExpression = false;
}


LowLevelCondition::~LowLevelCondition(void)
{
	//deleteVectorPointer(testedFlags); testedFlags = NULL;
}


std::string LowLevelCondition::getExpressionString(){
	std::string expString = "(";
	std::vector<FlagPtr>::iterator iter;
	for(iter = testedFlags->begin() ; iter != testedFlags->end() ; iter++){
		expString += (*iter)->getExpressionString();
		if(iter + 1 != testedFlags->end()){
			expString += (*iter)->getExpressionString() + ", ";
		}
	}
	expString += "), BranchType: " + getBranchTypeString();
	return expString;
}


char* LowLevelCondition::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* commaCOLSTR = getColoredString(", ", CODE_COLOR);
	char* branchTypeCOLSTR = getColoredString(getBranchTypeString(), CODE_COLOR);
	char* branchNameCOLSTR = getColoredString("), BranchType: ", CODE_COLOR);
	char** flagsCOLSTR = new char*[testedFlags->size()];
	
	int* flagsCOLSTRSize = new int[testedFlags->size()];
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int commaCOLSTRSize = strlen(commaCOLSTR);
	int branchTypeCOLSTRSize = strlen(branchTypeCOLSTR);
	int branchNameCOLSTRSize = strlen(branchNameCOLSTR);
	
	int expCOLSTRSize = firstBracketCOLSTRSize + branchNameCOLSTRSize + branchTypeCOLSTRSize;
	for(unsigned int i = 0 ; i < testedFlags->size() ; i++){
		flagsCOLSTR[i] = (*testedFlags)[i]->getColoredExpressionString();
		flagsCOLSTRSize[i] = strlen(flagsCOLSTR[i]);
		expCOLSTRSize += flagsCOLSTRSize[i];
		if(i != testedFlags->size() - 1){
			expCOLSTRSize += commaCOLSTRSize;
		}
	}

	char* expCOLSTR = (char*)malloc(expCOLSTRSize + 1);
	memcpy(expCOLSTR, firstBracketCOLSTR, firstBracketCOLSTRSize);
	int currentPosition = firstBracketCOLSTRSize;
	for(unsigned int i = 0 ; i < testedFlags->size() ; i++){
		memcpy(expCOLSTR + currentPosition, flagsCOLSTR[i], flagsCOLSTRSize[i]);
		currentPosition += flagsCOLSTRSize[i];
		if(i != testedFlags->size() - 1){
			memcpy(expCOLSTR + currentPosition, commaCOLSTR, commaCOLSTRSize);
			currentPosition += commaCOLSTRSize;
		}
		free(flagsCOLSTR[i]);
	}
	memcpy(expCOLSTR + currentPosition, branchNameCOLSTR, branchNameCOLSTRSize);
	currentPosition += branchNameCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, branchTypeCOLSTR, branchTypeCOLSTRSize);
	currentPosition += branchTypeCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);

	free(firstBracketCOLSTR);
	delete[] flagsCOLSTR;
	delete[] flagsCOLSTRSize;
	free(commaCOLSTR);
	free(branchNameCOLSTR);
	free(branchTypeCOLSTR);
	return expCOLSTR;
}


std::string LowLevelCondition::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append("(");
	
	for(std::vector<FlagPtr>::iterator iter = testedFlags->begin() ; iter != testedFlags->end() ; ++iter){
		coloredString.append((*iter)->getExpressionCOLSTR());
		if(iter + 1 != testedFlags->end())
			coloredString.append(", ");
	}
	coloredString.append("), BranchType: ");
	coloredString.append(getBranchTypeString());
	return coloredString;
}


//std::vector<Expression*>* LowLevelCondition::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->insert(expElements->end(), testedFlags->begin(), testedFlags->end());
//	return expElements;
//}

void LowLevelCondition::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	elements_out.insert(elements_out.end(), testedFlags->begin(), testedFlags->end());
}


ExpressionPtr LowLevelCondition::deepcopy(){
	FlagVectorPtr copyTestedFlags = std::make_shared<std::vector<FlagPtr>>();
	std::vector<FlagPtr>::iterator iter;
	for(iter = testedFlags->begin() ; iter != testedFlags->end() ; iter++){
		FlagPtr copyFlag = std::dynamic_pointer_cast<Flag>((*iter)->deepcopy());
		copyTestedFlags->push_back(copyFlag);
	}
	LowLevelConditionPtr copyPtr = std::make_shared<LowLevelCondition>(copyTestedFlags, branchType);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}


bool LowLevelCondition::replaceChildExpressionNoSimplification(const std::string& nameToBeReplaced, int subscriptToBeReplaced, const ExpressionPtr& replacingExpression){
	for(std::vector<FlagPtr>::iterator it = testedFlags->begin() ; it != testedFlags->end() ; ++it){
		if((*it)->isSameVariable(nameToBeReplaced, subscriptToBeReplaced)){
			(*it)->name = replacingExpression->getExpressionString();
			(*it)->subscript = NO_SUBSCRIPT;
			return true;
		}
	}
	return false;
}

bool LowLevelCondition::isUnsignedCompare(){
	return branchType == Below || branchType == Below_or_Equal || 
					  branchType == Above || branchType == Above_or_Equal;

	//bool isSigned = branchType == Less || branchType == Less_or_Equal || branchType == Greater || branchType == Greater_or_Equal;
}

std::string LowLevelCondition::getBranchTypeString(){
	switch(branchType){
	case Above:
		return "Above";
	case Above_or_Equal:
		return "Above_or_Equal";
	case Below:
		return "Below";
	case Below_or_Equal:
		return "Below_or_Equal";
	/*case Carry:
		return "Carry";*/
	case CX_Zero:
		return "CX_Zero";
	case Equal:
		return "Equal";
	case Greater:
		return "Greater";
	case Greater_or_Equal:
		return "Greater_or_Equal";
	case Less:
		return "Less";
	case Less_or_Equal:
		return "Less_or_Equal";
	/*case Not_Above:
		return "Not_Above";
	case Not_Above_or_Equal:
		return "Not_Above_or_Equal";
	case Not_Below:
		return "Not_Below";
	case Not_Below_or_Equal:
		return "Not_Below_or_Equal";
	case Not_Carry:
		return "Not_Carry";*/
	case Not_Equal:
		return "Not_Equal";
	/*case Not_Greater:
		return "Not_Greater";
	case Not_Greater_or_Equal:
		return "Not_Greater_or_Equal";
	case Not_Less:
		return "Not_Less";
	case Not_Less_or_Equal:
		return "Not_Less_or_Equal";*/
	case Not_Overflow:
		return "Not_Overflow";
	case Not_Parity:
		return "Not_Parity";
	case Not_Signed:
		return "Above_or_Equal";
	/*case Not_Zero:
		return "Above_or_Equal";*/
	case Overflow:
		return "Overflow";
	case Parity:
		return "Parity";
	/*case Parity_Even:
		return "Parity_Even";
	case Parity_Odd:
		return "Parity_Odd";*/
	case Signed:
		return "Signed";
	/*case Zero:
		return "Zero";*/
	default:
		msg("Flag Variable Error");
		return std::string("Flag Variable Error");
	}
}
