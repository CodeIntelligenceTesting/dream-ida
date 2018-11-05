/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once


enum SimplificationType{
	NO_SIMPLIFICATION,
	NEGATION_OF_ADDITION,							// -(a+b) = -a + -b, 
	NEGATION_OF_MULTIPLICATION,
	CONSTANT_NEGATION,								// -Num(n) = Num(-n)
	DOUBLE_NEGATION,								// -(-a) = a
	DOUBLE_LOGICAL_NOT,								// ~(~a) = a
	DOUBLE_EXPONENTIATION,
	EXPONENTIATION_OF_MYLTIPLICATION
	//DOUBLE_DIVISION_DIVIDEND_SIMPLE,				// (a/b)/c = (a*c)/b
	//DOUBLE_DIVISION_DIVIDEND_AGGREGATE,
	//DOUBLE_DIVISION_DIVISOR_SIMPLE,				// a/(b/c) = (a*c)/b
	//DOUBLE_DIVISION_DIVISOR_AGGREGATE,
	//MYLTIPLICATION_TO_DIVISION					// a*(c/d) = (a*c)/d
};
