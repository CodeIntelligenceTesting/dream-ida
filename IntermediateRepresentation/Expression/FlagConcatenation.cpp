////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "FlagConcatenation.h"
//#include "../../Shared/houseKeepingUtilities.h"

FlagConcatenation::FlagConcatenation(intToFlagMapPtr _flagMap) : Expression(BIT_CONCATENATION, NO_SUBSCRIPT)
{
	flagMap = _flagMap;
	isSimpleExpression = false;
}


FlagConcatenation::~FlagConcatenation(void)
{
	//deleteMapToPointers(flagMap); flagMap = NULL;
}


std::string FlagConcatenation::getExpressionString(){
	std::string flagMapString = "(";
	std::map<int, FlagPtr>::iterator iter;
	for(iter = flagMap->begin() ; iter != flagMap->end() ; iter++){
		flagMapString += iter->second->getExpressionString() + ", ";
	}

	return flagMapString.substr(0, flagMapString.size() - 2) + ")";
}


char* FlagConcatenation::getColoredExpressionString(){
	char* firstBracketCOLSTR = getColoredString("(", CODE_COLOR);
	char* secondBracketCOLSTR = getColoredString(")", CODE_COLOR);
	char* firstSquareBracketCOLSTR = getColoredString("[", CODE_COLOR);
	char* secondSquareBracketCOLSTR = getColoredString("]", CODE_COLOR);
	char* commaCOLSTR = getColoredString(", ", CODE_COLOR);

	char** flagsCOLSTR = new char*[flagMap->size()];
	char** flagsNoCOLSTR = new char*[flagMap->size()];
	int* flagsCOLSTRSize = new int[flagMap->size()];
	int* flagsNoCOLSTRSize = new int[flagMap->size()];
	
	int firstBracketCOLSTRSize = strlen(firstBracketCOLSTR);
	int secondBracketCOLSTRSize = strlen(secondBracketCOLSTR);
	int firstSquareBracketCOLSTRSize = strlen(firstSquareBracketCOLSTR);
	int secondSquareBracketCOLSTRSize = strlen(secondSquareBracketCOLSTR);
	int commaCOLSTRSize = strlen(commaCOLSTR);
	int expCOLSTRSize = firstSquareBracketCOLSTRSize + secondSquareBracketCOLSTRSize - commaCOLSTRSize + 
						flagMap->size() * (commaCOLSTRSize + firstBracketCOLSTRSize + secondBracketCOLSTRSize);

	int currentPosition = 0;
	std::map<int, FlagPtr>::iterator iter;
	for(iter = flagMap->begin() ; iter != flagMap->end() ; ++iter){
		flagsCOLSTR[currentPosition] = iter->second->getColoredExpressionString();
		flagsCOLSTRSize[currentPosition] = strlen(flagsCOLSTR[currentPosition]);

		flagsNoCOLSTR[currentPosition] = getColoredString(boost::lexical_cast<std::string>(iter->first), NUMBER_COLOR);
		flagsNoCOLSTRSize[currentPosition] = strlen(flagsNoCOLSTR[currentPosition]);
		expCOLSTRSize += flagsCOLSTRSize[currentPosition] + flagsNoCOLSTRSize[currentPosition];
		currentPosition++;
	}
	char* expCOLSTR = (char*)malloc(expCOLSTRSize + 1);
	memcpy(expCOLSTR, firstSquareBracketCOLSTR, firstSquareBracketCOLSTRSize);
	currentPosition = firstSquareBracketCOLSTRSize;
	for(unsigned int i = 0 ; i < flagMap->size() ; i++){
		memcpy(expCOLSTR + currentPosition, flagsCOLSTR[i], flagsCOLSTRSize[i]);
		
		currentPosition += flagsCOLSTRSize[i];
		memcpy(expCOLSTR + currentPosition, firstBracketCOLSTR, firstBracketCOLSTRSize);

		currentPosition += firstBracketCOLSTRSize;
		memcpy(expCOLSTR + currentPosition, flagsNoCOLSTR[i], flagsNoCOLSTRSize[i]);

		currentPosition += flagsNoCOLSTRSize[i];
		memcpy(expCOLSTR + currentPosition, secondBracketCOLSTR, secondBracketCOLSTRSize);

		currentPosition += secondBracketCOLSTRSize;
		if(i != flagMap->size() - 1){
			memcpy(expCOLSTR + currentPosition, commaCOLSTR, commaCOLSTRSize);
			currentPosition += commaCOLSTRSize;
		}

		free(flagsCOLSTR[i]);
		free(flagsNoCOLSTR[i]);
	}

	memcpy(expCOLSTR + currentPosition, secondSquareBracketCOLSTR, secondSquareBracketCOLSTRSize);

	currentPosition += secondSquareBracketCOLSTRSize;
	memcpy(expCOLSTR + currentPosition, "\0", 1);
	free(firstBracketCOLSTR);
	free(secondBracketCOLSTR);
	free(firstSquareBracketCOLSTR);
	free(secondSquareBracketCOLSTR);
	free(commaCOLSTR);
	delete[] flagsCOLSTR;
	delete[] flagsCOLSTRSize;
	delete[] flagsNoCOLSTR;
	delete[] flagsNoCOLSTRSize;
	return expCOLSTR;
}


std::string FlagConcatenation::getExpressionCOLSTR(){
	std::string coloredString;
	coloredString.append("(");
	for(std::map<int, FlagPtr>::iterator iter = flagMap->begin() ; iter != flagMap->end() ; iter++){
		coloredString.append(iter->second->getExpressionCOLSTR() + ", ");
	}
	return coloredString.substr(0, coloredString.size() - 2) + ")";
}


ExpressionPtr FlagConcatenation::deepcopy(){
	intToFlagMapPtr copyFlagMap = std::make_shared<std::map<int, FlagPtr>>();
	std::map<int, FlagPtr>::iterator iter;
	for(iter = flagMap->begin() ; iter != flagMap->end() ; iter++){
		FlagPtr copyFlag = std::dynamic_pointer_cast<Flag>(iter->second->deepcopy());
		copyFlagMap->operator[](iter->first) = copyFlag;
	}
	FlagConcatenationPtr copyPtr = std::make_shared<FlagConcatenation>(copyFlagMap);
	return copyPtr;
}


//std::vector<Expression*>* FlagConcatenation::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	std::map<int, Flag*>::iterator iter;
//	for(iter = flagMap->begin() ; iter != flagMap->end() ; iter++){
//		expElements->push_back(iter->second);
//	}
//	return expElements;
//}

void FlagConcatenation::getExpressionElements(bool includePointers, std::vector<ExpressionPtr>& elements_out){
	std::map<int, FlagPtr>::iterator iter;
	for(iter = flagMap->begin() ; iter != flagMap->end() ; iter++){
		elements_out.push_back(iter->second);
	}
}
