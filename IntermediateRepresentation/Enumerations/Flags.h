/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

enum FlagNo{
	CF,			// = 0,
	PF,			// = 2,
	AF,			// = 4,
	ZF,			// = 6,
	SF,			// = 7,
	DF,			// = 10,
	OF,			// = 11
	C0,
	C1,
	C2,
	C3
};

enum FlagType{
	CPU_FLAG,
	FPU_FLAG
};
