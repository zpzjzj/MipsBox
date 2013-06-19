/*
 * reg.c
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */
#include "memory.h"
#include "header.h"
#include "reg.h"
#include "GetStr.h"
#include "hash/HashQuad.h"
#include <stdio.h>
#include <string.h>

#define REG_BUG
const size_t MAX_REG_LEN = 10;
struct reg_table{
	char name[MAX_REG_LEN];
	size_t num;
};
static struct reg_table regTable[] = {
		{"$zero", 0},
		{"$at", 1},
		{"$v0", 2},
		{"$v1", 3},
		{"$a0", 4},
		{"$a1", 5},
		{"$a2", 6},
		{"$a3", 7},
		{"$t0", 8},
		{"$t1", 9},
		{"$t2", 10},
		{"$t3", 11},
		{"$t4", 12},
		{"$t5", 13},
		{"$t6", 14},
		{"$t7", 15},
		{"$s0", 16},
		{"$s1", 17},
		{"$s2", 18},
		{"$s3", 19},
		{"$s4", 20},
		{"$s5", 21},
		{"$s6", 22},
		{"$s7", 23},
		{"$t8", 24},
		{"$t9", 25},
		{"$k0", 26},
		{"$k1", 27},
		{"$gp", 28},
		{"$sp", 29},
		{"$fp", 30},
		{"$ra", 31},
		{"$sr", 12},
		{"$cr", 13},
		{"$epc",14},
//for immediate
		{"$0", 0},
		{"$1", 1},
		{"$2", 2},
		{"$3", 3},
		{"$4", 4},
		{"$5", 5},
		{"$6", 6},
		{"$7", 7},
		{"$8", 8},
		{"$9", 9},
		{"$10", 10},
		{"$11", 11},
		{"$12", 12},
		{"$13", 13},
		{"$14", 14},
		{"$15", 15},
		{"$16", 16},
		{"$17", 17},
		{"$18", 18},
		{"$19", 19},
		{"$20", 20},
		{"$21", 21},
		{"$22", 22},
		{"$23", 23},
		{"$24", 24},
		{"$25", 25},
		{"$26", 26},
		{"$27", 27},
		{"$28", 28},
		{"$29", 29},
		{"$30", 30},
		{"$31", 31}
};//reg table
	//use structure instead of array for clear to see

const size_t tableSize = sizeof(regTable)/sizeof(struct reg_table);

HashTable regHashTbl = NULL;
int *regStrFindTbl;//table[hash(label)] = i;

void InitRegTable(void){
	regHashTbl =  InitializeTable(tableSize * 2);//create hash table
	size_t HashTableSize = regHashTbl->TableSize;
	regStrFindTbl = (int *)malloc(sizeof(size_t) * HashTableSize);
	memset(regStrFindTbl, NONE, sizeof(size_t) * HashTableSize);
//create table
	Position pos;
	size_t i = 0;
	for(i = 0; i < tableSize; i++){//for every index
		pos = Insert(regTable[i].name, regHashTbl);//insert op_str to table
		regStrFindTbl[pos] = i;
	}
}

void DestroyRegTable(void){
	free(regStrFindTbl);
	DestroyTable(regHashTbl);
}
char* GetRegFieldStr(char *command, reg_type *ptr_reg){
	//get the reg's string representation from command
	reg_type GetRegCode(char *strReg);
	char str_reg[MAX_REG_LEN];
	char *oldCommandPtr = command;//for saving old command
	if((command = GetStr(str_reg, command)) == oldCommandPtr){
		ERROR("Wrong register format.\n");
	}
	else *ptr_reg = GetRegCode(str_reg);
	return command;
}

reg_type GetRegCode(char *strReg){
	//get code according to its string
	Position pos;
	pos = Find(strReg, regHashTbl);
	if(regStrFindTbl[pos] == NONE){
		ERROR("Wrong Register");
		return 0;
	}
	else{
		size_t index = regStrFindTbl[pos];
		return regTable[index].num;
	}
}


void GetRegStr(char *strReg, reg_type reg){
	//get string according to the field
	//if null return ""
	if(reg < tableSize)
		strcpy(strReg, regTable[reg].name);
	else
		strcpy(strReg, "?");
}
