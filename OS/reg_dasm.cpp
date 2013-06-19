/*
 * reg.c
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */
#include "memory.h"
#include "header.h"
#include "GetStr.h"
#include <stdio.h>
#include <string.h>

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
		{"$ra", 31}
};//reg table
	//use structure instead of array for clear to see

const size_t tableSize = sizeof(regTable)/sizeof(struct reg_table);

void GetRegStr(char *strReg, reg_type reg){
	//get string according to the field
	//if null return ""
	if(reg < tableSize)
		strcpy(strReg, regTable[reg].name);
	else
		strcpy(strReg, "?");
}

