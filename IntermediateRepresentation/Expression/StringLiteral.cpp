////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "StringLiteral.h"
#include <ctype.h>
#include <boost/format.hpp>

#include "../../TypeAnalysis/Types/SimpleType/IntegralType.h"
#include "../../TypeAnalysis/Types/SimpleType/PointerType.h"

StringLiteral::StringLiteral(std::string _value) : Expression(STRING_LITERAL, NO_SUBSCRIPT)
{
	value = replaceEscapeCharacters(_value);
	isSimpleExpression = true;
	PointerTypePtr pointerToCharType = std::make_shared<PointerType>();
	pointerToCharType->pointedToType = std::make_shared<IntegralType>(CHAR_TYPE);
	expressionType = pointerToCharType;
}


StringLiteral::~StringLiteral(void)
{
}

std::string StringLiteral::getExpressionString(){
	return "\"" + value + "\"";
}

char* StringLiteral::getColoredExpressionString(){
	return getColoredString(getExpressionString(), STRING_COLOR);
}

std::string StringLiteral::getExpressionCOLSTR(){
	return getCOLSTR(getExpressionString(), STRING_COLOR);
}

std::string StringLiteral::to_json(){
	return "{\"expression_type\":\"StringLiteral\", \"value\":\"" + value + "\"}";
}

//std::vector<Expression*>* StringLiteral::getExpressionElements(bool includePointers){
//	std::vector<Expression*>* expElements = new std::vector<Expression*>();
//	expElements->push_back(this);
//	return expElements;
//}

ExpressionPtr StringLiteral::deepcopy(){
	StringLiteralPtr copyPtr = std::make_shared<StringLiteral>(value);
	copyPtr->size_in_bytes = size_in_bytes;
	copyPtr->expressionType = expressionType->deepcopy();
	return copyPtr;
}

std::string StringLiteral::replaceEscapeCharacters(std::string str){
	std::string escapeString;
	for(std::string::iterator char_iter = str.begin() ; char_iter != str.end() ; ++char_iter){
		char currentChar = *char_iter;
		switch(currentChar){
		case '\n':
			escapeString += "\\n";
			break;
		case '\r':
			escapeString += "\\r";
			break;
		case '\t':
			escapeString += "\\t";
			break;
		case '\\':
			escapeString += "\\\\";
			break;
		case '\v':
			escapeString += "\\v";
			break;
		case '\b':
			escapeString += "\\b";
			break;
		case '\f':
			escapeString += "\\f";
			break;
		case '\a':
			escapeString += "\\a";
			break;
		case '\"':
			escapeString += "\\\"";
			break;
		default:
			if(__isascii(currentChar)){
				escapeString += currentChar;
			}
			else{
				escapeString += boost::str((boost::format("\\x%02x") % ((unsigned int)(currentChar & 0xFF))));
			}
		}
						
	}
	return escapeString;
}

TypePtr StringLiteral::getExpresstionType(TypeMapPtr typeMap){
	return expressionType;
}
