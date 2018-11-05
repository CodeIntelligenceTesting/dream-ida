////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Jump.h"


Jump::Jump(InstructionType _instructionType) : UnaryInstruction(_instructionType)
{

}


Jump::~Jump(void)
{
}

std::string Jump::getInstructionString(){
	return "Jump " + getTargetString();
}

char* Jump::getColoredInstructionString(){
	char* jumpCOLSTR = getColoredString("Jump ", CODE_COLOR);
	char* operandCOLSTR = getTargetColoredString();

	int jumpCOLSTRSize = strlen(jumpCOLSTR);
	int operandCOLSTRSize = strlen(operandCOLSTR);

	char* instCOLSTR = (char*)malloc(jumpCOLSTRSize + operandCOLSTRSize + 1);
	memcpy(instCOLSTR, jumpCOLSTR, jumpCOLSTRSize);

	int currentPosition = jumpCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, operandCOLSTR, operandCOLSTRSize);

	currentPosition += operandCOLSTRSize;
	memcpy(instCOLSTR + currentPosition, "\0", 1);

	free(jumpCOLSTR);
	free(operandCOLSTR);

	return instCOLSTR;
}

std::string Jump::getInstructionCOLSTR(){
	std::string coloredString;
	coloredString.append("jump ");
	coloredString.append(getTargetCOLSTR());
	return coloredString;
}

/*std::string Jump::to_json(){
	return "{\"instruction_type\":\"Jump\"}";
}*/

//std::vector<Expression*>* Jump::getUsedElements(){
//	return NULL;
//}
//
//std::vector<Expression*>* Jump::getDefinedElements(){
//	return NULL;
//}
