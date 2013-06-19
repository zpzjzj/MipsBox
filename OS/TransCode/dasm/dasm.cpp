/*
 * instruction.c
 *
 *  Created on: Apr 14, 2013
 *      Author: zhaoping
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dasm.h"
#include "..\instruction.h"
#include "..\..\reg_dasm.h"
#include "..\..\header.h"

//length of opcode
//#define OP_CODE_LEN 6
void DisassembleCode(char *instruction, code_type code, pc_type pc){
	//transfer from code to instruction
	void I_typeUnasm(code_type code, reg_type *ptr_rs, reg_type *ptr_rt, reg_type *ptr_immediate);
	void R_typeUnasm(code_type code, reg_type *ptr_rs, reg_type *ptr_rt, reg_type *ptr_rd, reg_type *ptr_shamt);
	instr_type GetType(char *type);

	int flag = 0;//for parentheses
	reg_type op = 0, rs = 0, rd = 0, rt = 0, shamt = 0, func = 0, immediate = 0, target = 0;//default num
	char str_field[MAX_INSTR_FIELD_STR_LEN];//string of field of instruction interpreted

	op = GetOP(code);//get opcode and eliminate it from code
	func = GetFunc(code);
	size_t instrIndex;
	if(op != 0){
		instrIndex = opCodeFindTbl[op];
	}
	else{
		instrIndex = functCodeFindTbl[func];
	}
	if(instrIndex == NONE){
		//can not find op
		strcpy(instruction, "???");
	}
	else{//found
		char *instrField = instrTable[instrIndex].instruction[1];
		//string in the field of instruction
		strcpy(instruction, instrField);//op
		strcat(instruction, " ");
		instr_type type;//type of the instruction
		type = GetType(instrTable[instrIndex].instruction[0]);

		switch(type){
		case R:
			R_typeUnasm(code, &rs, &rt, &rd, &shamt);
			break;
		case I:
			I_typeUnasm(code, &rs, &rt, &immediate);
			break;
		case J:
			target = GetTarget(code);//J type
			break;
		}

		size_t cnt = 0, i = 0;
		for(i = 2; *instrTable[instrIndex].instruction[i] != 0; i++) ;
		cnt = i;//number of fields

		for(i = 2; *instrTable[instrIndex].instruction[i] != 0; i++){
			//get data from code according to grammar (rs, rd, rt, shamt, immediate)
			instrField = instrTable[instrIndex].instruction[i];
			if(strcmp(instrField, "rs") == 0){
				GetRegStr(str_field, rs);
			}
			else if(strcmp(instrField, "rt") == 0){
				GetRegStr(str_field, rt);
			}
			else if(strcmp(instrField, "rd") == 0){
				GetRegStr(str_field, rd);
			}
			else if(strcmp(instrField, "sa") == 0){
				sprintf(str_field, "%d", shamt);
			}
			else if(strcmp(instrField, "immediate") == 0){//immediate
				sprintf(str_field, "%d", immediate);
			}
			else if(strcmp(instrField, "label") == 0){
				if(type == I){
					if(immediate >= 0X8000){//negative
						immediate = (immediate ^ 0X0000FFFF) + 1;//trans_complement of least 16 bits
						immediate = (((pc + 4) >> 2) - immediate) << 2;
					}
					else
						immediate = (int32_t)(immediate << 2) + (pc + 4);
					sprintf(str_field, "%d", immediate);
				}
				else if(type == J){
					target <<= 2;
					target |= (0XF0000000 & (pc + 4));
					sprintf(str_field, "%d", target);
				}
			}//end of if
			strcat(instruction, str_field);
			if(strcmp(instrField, "immediate") == 0 && i == cnt - 2){//immediate(rs) type
				strcat(instruction, "(");
				flag = 1;
			}
			else if(flag == 1){//immediate(rs) type
				strcat(instruction, ")");
				flag = 0;
			}
			else if(i < cnt - 1)
				strcat(instruction, ", ");
		}//end of for
	}
}

void R_typeUnasm(code_type code, reg_type *ptr_rs, reg_type *ptr_rt, reg_type *ptr_rd, reg_type *ptr_shamt){
	*ptr_rs = (code >> (REG_CODE_LEN * 2 + SHAMT_CODE_LEN + FUNC_CODE_LEN)) & 0X1F;
	*ptr_rt = (code >> (REG_CODE_LEN + SHAMT_CODE_LEN + FUNC_CODE_LEN)) & 0X1F;
	*ptr_rd = (code >> (SHAMT_CODE_LEN + FUNC_CODE_LEN)) & 0X1F;
	*ptr_shamt = GetShamt(code);
}

#define IMMED_CODE_LEN 16
void I_typeUnasm(code_type code, reg_type *ptr_rs, reg_type *ptr_rt, reg_type *ptr_immediate){
	*ptr_rs = (code >> (REG_CODE_LEN + IMMED_CODE_LEN)) & 0X1F;//five bits
	*ptr_rt = (code >> IMMED_CODE_LEN) & 0X1F;
	*ptr_immediate = code & 0XFFFF;//16 bits
}
