/*
 * instruction.c
 *
 *  Created on: Apr 14, 2013
 *      Author: zhaoping
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dasm_dasm.h"
#include "..\dasm_instruction.h"
#include "..\..\dasm_reg.h"
#include "..\..\dasm_header.h"

//#define DASM_BUG
//length of opcode
void DisassembleCode(char *instruction, const code_type code, pc_type pc){
	//transfer from code to instruction
	void I_typeUnasm(code_type code, reg_type *ptr_rs, reg_type *ptr_rt, reg_type *ptr_immediate);
	void R_typeUnasm(code_type code, reg_type *ptr_rs, reg_type *ptr_rt, reg_type *ptr_rd, reg_type *ptr_shamt);
	instr_type GetType(char *type);

	int flag = 0;//for parentheses
	char str_field[MAX_INSTR_FIELD_STR_LEN];//string of field of instruction interpreted

	reg_type op = GetOP(code);//get opcode and eliminate it from code
	reg_type func = GetFunc(code);
	reg_type target = GetTarget(code);
	reg_type shamt = GetShamt(code);
	reg_type immediate = code & 0XFFFF;
	reg_type rs, rt, rd;
	{
		code_type tmp = code;
		tmp >>= (SHAMT_CODE_LEN + FUNC_CODE_LEN);
		size_t pos = REG_CODE_LEN * 2;
		rs = (tmp & (0X1F << pos)) >> pos;
		pos -= REG_CODE_LEN;
		rt = (tmp & (0X1F << pos)) >> pos;
		rd = tmp & 0X1F;
	}//get rs rt rd

	size_t instrIndex = NONE;
	if(op != 0 && op != 16){//except R type
		instrIndex = opCodeFindTbl[op];
	}
	else{//op == 0 or op == 16
		if(op == 0)
			instrIndex = functCodeFindTbl[func];
		else{//op == 16
			char rs_str[MAX_INSTR_FIELD_STR_LEN];
			itoa(rs, rs_str, 10);
			char func_str[MAX_INSTR_FIELD_STR_LEN];
			itoa(func, func_str, 10);
#ifdef DASM_BUG
			printf("rs_str: %s\n", rs_str);
			printf("func_str: %s\n", func_str);
#endif
			for(size_t i = 0; i < instrNum; i++){
#ifdef DASM_BUG
			printf("instrTable[%d].code[0]: %s\n", i, instrTable[i].code[0]);
			printf("instrTable[%d].code[1]: %s\n", i, instrTable[i].code[1]);
			printf("instrTable[%d].code[5]: %s\n", i, instrTable[i].code[5]);
#endif
				if(strcmp(instrTable[i].code[0], "16") == 0 
					&& strcmp(instrTable[i].code[1], rs_str) == 0
					&& strcmp(instrTable[i].code[5], func_str) == 0){
						instrIndex = i;
						break;
				}
			}
		}//end of else
	}//end of op == 16 or 0

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

		size_t cnt = 0, i = 0;
		for(i = 2; *instrTable[instrIndex].instruction[i] != 0; i++) ;
		cnt = i;//number of fields

		for(i = 2; *instrTable[instrIndex].instruction[i] != 0; i++){
			//get data from code according to grammar (rs, rd, rt, shamt, immediate)
			instrField = instrTable[instrIndex].instruction[i];
			if(strcmp(instrField, "rs") == 0){
				GetRegStr(str_field, rs, op, RS);
			}
			else if(strcmp(instrField, "rt") == 0){
				GetRegStr(str_field, rt, op, RT);
			}
			else if(strcmp(instrField, "rd") == 0){
				GetRegStr(str_field, rd, op, RD);
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
