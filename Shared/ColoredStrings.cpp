////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "ColoredStrings.h"

char* getColoredString(std::string uncoloredString, const char* colorTag){
	int uncoloredStringStringSize = uncoloredString.size();
	//int colorTagSize = strlen(colorTag);
	char* coloredInstString = (char*)malloc(uncoloredStringStringSize + 5);
	
	memcpy(coloredInstString, "\1", 1);
	memcpy(coloredInstString + 1, colorTag, 1);
	memcpy(coloredInstString + 2, uncoloredString.c_str(), uncoloredStringStringSize);
	memcpy(coloredInstString + 2 + uncoloredStringStringSize, "\2", 1);
	memcpy(coloredInstString + 3 + uncoloredStringStringSize, colorTag, 1);
	memcpy(coloredInstString + 4 + uncoloredStringStringSize, "\0", 1);
	
	return coloredInstString;
}

std::string getCOLSTR(std::string uncoloredString, const char* colorTag){
	std::string coloredString;
	coloredString.push_back(COLOR_ON);
	coloredString.append(colorTag);
	coloredString.append(uncoloredString);
	coloredString.push_back(COLOR_OFF);
	coloredString.append(colorTag);
	return coloredString;
}
