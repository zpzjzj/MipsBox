/*
 * instruction.c
 *
 *  Created on: Apr 21, 2013
 *      Author: zhaoping
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "..\GetStr.h"
#include "..\header.h"

//#define INSTR_BUG
struct instruction_table instrTable[MAX_INSTR_NUM];
size_t instrNum = 0;//number of available instructions in the table
HashTable instrHashTbl = NULL;
int *opStrHashFindTbl;//tableSize
//table[hash("add")]->instructTable[i][]
int opCodeFindTbl[OP_CODE_NUM];
//table[opcode]->instructTable[i][]
int functCodeFindTbl[FUNCT_CODE_NUM];
//table[funct]->instructTable[i][]
void ReadInstr(void){
	//read in instructions to global table
	void InitInstrTable(void);
	memset(instrTable, 0, sizeof(struct instruction_table) * MAX_INSTR_NUM);
	FILE* pFile;
	if((pFile = fopen("instruction.txt", "r")) == NULL)
		ERROR("Cannot open the instruction.txt file\n");

	size_t i = 0, j = 0, count = 0;

	char str[MAX_INSTR_STR_LEN];
	char *line;
	while(fgets(str, MAX_INSTR_STR_LEN, pFile) != NULL){//get in string
		//for every line
		line = str;
		char *oldLinePtr = line;
		j = 0;
		if(count % 2 == 0){//instruction
			while((line = GetStr(instrTable[i].instruction[j], line)) != oldLinePtr){
				//extract a string
				j++;
				oldLinePtr = line;
			}
		}
		else{//code
			while((line = GetStr(instrTable[i].code[j], line)) != oldLinePtr){//get a string
				j++;
				oldLinePtr = line;
			}
		}
		count++;
		i = count / 2;
	}
	instrNum = i;
	InitInstrTable();
	fclose(pFile);
}

void InitInstrTable(void){
	instrHashTbl =  InitializeTable(instrNum * 2);//create hash table
	size_t tableSize = instrHashTbl->TableSize;
	opStrHashFindTbl = (int *)malloc(sizeof(size_t) * tableSize);
	memset(opStrHashFindTbl, NONE, sizeof(size_t) * tableSize);
	memset(opCodeFindTbl, NONE, sizeof(size_t) * OP_CODE_NUM);
	memset(functCodeFindTbl, NONE, sizeof(size_t) * FUNCT_CODE_NUM);
//create table

	Position pos;
	reg_type op = 0;
	reg_type funct = 0;

	size_t i = 0;
	for(i = 0; i < instrNum; i++){
		pos = Insert(instrTable[i].instruction[1], instrHashTbl);//insert op_str to table
		opStrHashFindTbl[pos] = i;
		sscanf(instrTable[i].code[0], "%d", &op);
		sscanf(instrTable[i].code[5], "%d", &funct);
		if(op != 0){
			opCodeFindTbl[op] = i;
		}
		else{
			functCodeFindTbl[funct] = i;
		}

#ifdef INSTR_BUG
		printf("pos: %d, retrieve: %s, opstr: %s\n",pos, Retrieve(pos, instrHashTbl),instrTable[i].instruction[1]);
#endif
	}
}

void DestroyInstrTable(void){
	free(opStrHashFindTbl);
	DestroyTable(instrHashTbl);
}
