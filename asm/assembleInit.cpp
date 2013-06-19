/*
 * assembleInit.c
 *
 *  Created on: May 4, 2013
 *      Author: zhaoping
 */

#include "TransCode/asm/Assemble.h"

#include "TransCode/asm/pseudo_instr.h"
#include "TransCode/instruction.h"
#include <string.h>
#include "reg.h"
#include "assembleInit.h"

void Init(void){
	InitRegTable();
	ReadInstr();
#ifdef MAIN_ERROR
	puts("after readInstr before readPseudoInstr");
#endif
	ReadPseudoInstr();
}

void ReleaseMem(void){
	DestroyInstrTable();
	DestroyRegTable();
}
