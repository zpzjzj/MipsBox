/*
 * Disassemble.c
 *
 *  Created on: Apr 17, 2013
 *      Author: zhaoping
 */
#include "dasm_dasm.h"
#include "..\..\dasm_codeType.h"
#include "..\dasm_instruction.h"
#include "..\..\dasm_reg.h"
#include <stdio.h>
#define printInstr(X, Y){printf("%04X:\t%s\n", (X), (Y));}

void Disassemble(code_type code, pc_type address){
	ReadInstr();//init
	char str_instruction[MAX_INSTR_STR_LEN];

	DisassembleCode(str_instruction, code, address);
	printInstr(address, str_instruction);
}