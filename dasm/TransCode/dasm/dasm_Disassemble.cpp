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

void Disassemble(code_type code[], size_t cnt, pc_type pc_start){
	ReadInstr();//init
	address_type address = pc_start;
	code_type instruction;
	char str_instruction[MAX_INSTR_STR_LEN];

	for(size_t i = 0; i < cnt; i++){
		instruction = code[i];//instruction 32 bit
		DisassembleCode(str_instruction, instruction, address);
		printInstr(address, str_instruction);
		address += 4;
	}
}