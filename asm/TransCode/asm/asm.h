/*
 * instruction.h
 *
 *  Created on: Apr 17, 2013
 *      Author: zhaoping
 */

#ifndef ASM_H_
#define ASM_H_

#include <stdint.h>
#include "..\..\codeType.h"
#define MAX_INSTR_STR_LEN 80 //max length of string of instruction (a line)

int AssembleCode(char *str_op, char * command, pc_type PC);
extern void ReadInstr(void);

#endif /* INSTRUCTION_H_ */
