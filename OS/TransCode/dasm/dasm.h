/*
 * dasm.h
 *
 *  Created on: Apr 14, 2013
 *      Author: zhaoping
 */
#ifndef DASM_H_
#define DASM_H_

#include <stdint.h>
#include <stdlib.h>
#include "..\..\codeType.h"

//length of instruction's code
#define CODE_LEN 32

#define OP_CODE_POS 26

#define SHAMT_CODE_POS 6
#define SHAMT_CODE_LEN 5
#define REG_CODE_LEN 5
#define FUNC_CODE_LEN 6

#define GetOP(X) (((X) >> OP_CODE_POS)&0X3F)
//get opcode
#define GetFunc(X) ((X)&0X3F)
#define GetTarget(X) ((X) & 0X3FFFFFF)
#define GetShamt(X) ((X)>>SHAMT_CODE_POS)&0X3F

void DisassembleCode(char *instruction, code_type code, pc_type pc);

#endif /* INSTRUCTION_H_ */


