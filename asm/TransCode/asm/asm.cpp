/*
 * instruction.c
 *
 *  Created on: Apr 17, 2013
 *      Author: zhaoping
 */

#include "..\..\reg.h"
#include "label.h"
#include "asm.h"
#include "Assemble.h"
#include "..\..\GetStr.h"
#include "..\instruction.h"
#include "..\..\hash\HashQuad.h"
#include "..\..\header.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/*
 * in instruction can have rt, rs, rd, sa, immediate, label
 * other than op
 * */
//#define ASM_BUG
int AssembleCode(char *str_op, char * command, pc_type PC){
	//translate command into code and store it
	//here command has extracted the operation field
	code_type GetRCode(size_t instrIndex, reg_type rs, reg_type rt, reg_type rd, reg_type shamt);
	code_type GetJCode(size_t instrIndex, reg_type immediate);
	code_type GetICode(size_t instrIndex, reg_type rs, reg_type rt, reg_type immediate);
	instr_type GetType(char *str_type);

	int flag = 0;
	reg_type rs = 0, rd = 0, rt = 0, immediate = 0, shamt = 0;//default num
	char str_field[MAX_INSTR_FIELD_STR_LEN]; //the string represent

	size_t pos;
	pos = Find(str_op, instrHashTbl);
	char *str = Retrieve(pos, instrHashTbl);
#ifdef ASM_BUG
	printf("pos: %d\n", pos);
#endif
	if(str == NULL){
		ERROR("Can not find op\n");
	}
	//find the op
	pos = opStrHashFindTbl[pos];
	size_t instrIndex = pos;//the instruction index
	char *instrField = instrTable[instrIndex].instruction[0];
	//string in the field of instruction
	instr_type type;
	type = GetType(instrField);
	char *oldCommandPtr;//for saving old command
	size_t i = 0;
	for(i = 2; *instrTable[instrIndex].instruction[i] != 0; i++){
		//get data from instruction according to format (rs, rd, rt, shamt, immediate)
		instrField = instrTable[instrIndex].instruction[i];
		oldCommandPtr = command;

		if(strcmp(instrField, "rs") == 0){
			command = GetRegFieldStr(command, &rs);
		}
		else if(strcmp(instrField, "rt") == 0){
			command = GetRegFieldStr(command, &rt);
		}
		else if(strcmp(instrField, "rd") == 0){
			command = GetRegFieldStr(command, &rd);
		}
		else if(strcmp(instrField, "sa") == 0){//shamt
			if((command = GetStr(str_field, command)) == oldCommandPtr) goto ERROR_INPUT;
			if(sscanf(str_field, "%u", &shamt) == 0) goto ERROR_INPUT;
		}
		else if(strcmp(instrField, "immediate") == 0){//immediate
			if((command = GetStr(str_field, command)) == oldCommandPtr)
				goto ERROR_INPUT;
			if(sscanf(str_field, "%u", &immediate) == 0)
				goto ERROR_INPUT;
		}
		else if(strcmp(instrField, "label") == 0){
			if((command = GetStr(str_field, command)) == oldCommandPtr)
				goto ERROR_INPUT;
			if(sscanf(str_field, "%u", &immediate) == 1){
				//if label is an immediate number
			}
			else{//alphabetical label
				if((FindLabel(str_field, &immediate)) == 0){//can not find the immediate
					AddLabelRecord(str_field, PC, type);
					immediate = 0;
				}
			}
			if(type == I)
				immediate = ((int32_t)(immediate - (PC + 4)) >> 2);
			else if(type == J){
				immediate &= 0X0FFFFFFF;//clear the highest 4 bits
				immediate >>= 2;
			}
		}//end of if
	}//end of for

	//modify according to encode data
	//suppose rs rt rd in order
	if(strcmp(instrTable[instrIndex].code[1], "rs") != 0)
		rs = atoi(instrTable[instrIndex].code[1]);
	if(strcmp(instrTable[instrIndex].code[2], "rt") != 0)
		rt = atoi(instrTable[instrIndex].code[2]);
	if(strcmp(instrTable[instrIndex].code[3], "rd") != 0)
		rd = atoi(instrTable[instrIndex].code[3]);
	if(strcmp(instrTable[instrIndex].code[4], "sa") != 0)
		shamt = atoi(instrTable[instrIndex].code[4]);

	//form code in format and get op, func according to code
	code_type code = 0;
	if(type == R) code = GetRCode(instrIndex, rs, rt, rd, shamt);
	else if(type == I) code = GetICode(instrIndex, rs, rt, immediate);
	else if(type == J) code = GetJCode(instrIndex, immediate);
	else ERROR("Wrong type\n");
	AddCommandCode(PC, code, type); //add the code transferred from command
	return flag;

	ERROR_INPUT:
	ERROR("Wrong input\n");
	return 0;
}

instr_type GetType(char *str_type){//type decide
	instr_type type;
	if(strcmp(str_type, "R") == 0){
		type = R;
	}
	else if(strcmp(str_type, "I") == 0){
		type = I;
	}
	else if(strcmp(str_type, "J") == 0){
		type = J;
	}
	else ERROR("Wrong Type\n");

	return type;
}

static const size_t RS_BINARY_LEN = 5;

code_type GetRCode(size_t instrIndex, reg_type rs, reg_type rt, reg_type rd, reg_type shamt){
	reg_type op = atoi(instrTable[instrIndex].code[0]);
	const size_t SHAMT_BINARY_LEN = 5;
	const size_t FUNCT_BINARY_LEN = 6;
	code_type code = 0;
	reg_type func = 0;//get func
	func = atoi(instrTable[instrIndex].code[5]);
	
	code += op & 0X3F;
	code <<= RS_BINARY_LEN;
	code += rs & 0X1F;
	code <<= RS_BINARY_LEN;
	code += rt & 0X1F;
	code <<= RS_BINARY_LEN;
	code += rd & 0X1F;
	code <<= SHAMT_BINARY_LEN;
	code += shamt & 0X1F;
	code <<= FUNCT_BINARY_LEN;
	code += func & 0X3F;

	return code;
}

code_type GetICode(size_t instrIndex, reg_type rs, reg_type rt, reg_type immediate){
	const size_t IMMEDIATE_BINARY_LEN = 16;
	code_type code = 0;
	reg_type op = 0;//get func
	op = atoi(instrTable[instrIndex].code[0]);

	code += op & 0X3F;

	code <<= RS_BINARY_LEN;
	code += rs & 0X1F;

	code <<= RS_BINARY_LEN;
	code += rt & 0X1F;

	code <<= IMMEDIATE_BINARY_LEN;
	code += immediate & 0XFFFF;

	return code;
}

code_type GetJCode(size_t instrIndex, reg_type immediate){
	const size_t TARGET_BINARY_LEN = 26;
	code_type code = 0;
	reg_type op = 0;//get func
	op = atoi(instrTable[instrIndex].code[0]);

	code += op & 0X3F;
	code <<= TARGET_BINARY_LEN;
	code += immediate & 0X3FFFFFF;

	return code;
}