/*
 * instruction.h
 *
 *  Created on: Apr 21, 2013
 *      Author: zhaoping
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <stddef.h>
#include "..\lenDef.h"

#define NONE -1
#define MAX_CODE_FIELD_LEN 27 //max field length in code(target->26)
#define MAX_CODE_FIELD_NUM 6//max field number in the code(op rs rt rd shamt func)
#define MAX_INSTR_NUM 200//max instruction lines in the text
#define MAX_INSTR_STR_LEN 80 //max length of string of instruction (a line)
#define MAX_INSTR_FIELD_STR_LEN 20//max length of string in a field of instruction
#define MAX_INSTR_FIELD_NUM 6//max argument number in the instruction

extern struct instruction_table{
	char instruction[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN];//type, operation and other fields
	//add type field for easy to print
	char code[MAX_CODE_FIELD_NUM][MAX_CODE_FIELD_LEN];//the value in decimal or define the format
}instrTable[MAX_INSTR_NUM];
extern size_t instrNum;

#include "..\hash\HashQuad.h"
extern HashTable instrHashTbl;
#define OP_CODE_NUM 128
#define FUNCT_CODE_NUM 127
extern int *opStrHashFindTbl;//table[hash("add")]->instructTable[i][]
extern int opCodeFindTbl[OP_CODE_NUM];
//table[opcode]->instructTable[i][]
extern int functCodeFindTbl[FUNCT_CODE_NUM];
//table[funct]->instructTable[i][]
extern int *opStrHashFindTbl;

extern void ReadInstr(void);
extern void InitInstrTable(void);
extern void DestroyInstrTable(void);
#endif /* INSTRUCTION_H_ */
