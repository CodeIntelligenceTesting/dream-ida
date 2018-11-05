/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

enum IntegralTypeEnum{
	SIZE8_TYPE,
		CHAR_TYPE,
		BOOL_TYPE,
	SIZE16_TYPE,
		SHORT_TYPE,
	SIZE32_TYPE,
		FLOAT_TYPE,
		POINTER_OR_INTEGER_TYPE,
			INT_TYPE,
			//POINTER_TYPE,
	SIZE64_TYPE,
		DOUBLE_TYPE,
		LONG_LONG_TYPE,
	LONG_DOUBLE_TYPE
};
