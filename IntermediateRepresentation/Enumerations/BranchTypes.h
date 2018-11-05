/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

enum BranchType{
	Above,					//CF=0 and ZF=0
	Above_or_Equal,			//CF=0
    Below,					//CF=1
    Below_or_Equal,			//CF=1 or ZF=1
    //Carry,				//CF=1
    CX_Zero,				//CX=0
    Equal,					//ZF=1
    Greater,				//ZF=0 and SF=OF	(signed)
    Greater_or_Equal,		//SF=OF				(signed)
    Less,					//SF != OF			(signed)
    Less_or_Equal,			//ZF=1 or SF != OF	(signed)
    //Not_Above,			//CF=1 or ZF=1
    //Not_Above_or_Equal,	//CF=1
    //Not_Below,			//CF=0
    //Not_Below_or_Equal,	//CF=0 and ZF=0
    //Not_Carry,			//CF=0
    Not_Equal,				//ZF=0
    //Not_Greater,			//ZF=1 or SF != OF	(signed)
    //Not_Greater_or_Equal,	//SF != OF			(signed)
    //Not_Less,				//SF=OF				(signed)
    //Not_Less_or_Equal,	//ZF=0 and SF=OF	(signed)
    Not_Overflow,			//OF=0				(signed)
    Not_Parity,				//PF=0
    Not_Signed,				//SF=0				(signed)
    //Not_Zero,				//ZF=0
    Overflow,				//OF=1				(signed)
    Parity,					//PF=1
    //Parity_Even,			//PF=1
    //Parity_Odd,			//PF=0
    Signed//,				//SF=1				(signed)
    //Zero					//ZF=1
};
