/*
 * proceed.c
 *
 *  Created on: Apr 21, 2013
 *      Author: zhaoping
 */
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "dasm/TransCode/dasm/dasm_dasm.h"
#include "arithmetic/iadder.h"
#include "arithmetic/imulti.h"
#include "reg.h"
#include "proceed.h"

#include "dasm/dasm_reg.h"
#include "dasm/TransCode/dasm/dasm_Disassemble.h"

//#define PROCEED_ERROR
void HandleException(address_type *ptr_address){
	//set EPC , disable exception_level bit, jump to EXCEPTION_ADDR
	cpo_reg[EPC] = *ptr_address;//save the pc of next instr
	ClearBit(&cpo_reg[STATUS], EXCEPTION_LEVEL); //disable EXL bit
	*ptr_address = EXCEPTION_ADDR;
}

void HandleRType(reg_type op, reg_type rs, reg_type rt, reg_type rd,
				 reg_type shamt, reg_type funct){//handle general R type code
	//handle general R instructions
	//except syscall and jump

	operand_type of, cf;
	switch(funct){
	case 42://slt
		reg[rd] = (reg[rs] < reg[rt]);
		break;
	case 32://add
		reg[rd] = __iadd(reg[rs], reg[rt], &cf, &of);
		break;
	case 34://sub
		reg[rd] = __isub(reg[rs], reg[rt], &cf, &of);
		break;
	case 36://and
		reg[rd] = reg[rs] & reg[rt];
		break;
	case 37://or
		reg[rd] = reg[rs] | reg[rt];
		break;
	case 38://xor
		reg[rd] = reg[rs] ^ reg[rt];
		break;
	case 0://sll
		reg[rd] = reg[rs] << shamt;
		break;
	case 27://divu
		cpo_reg[ENTRY_LO] = reg[rs] / reg[rt];
		cpo_reg[ENTRY_HI] = reg[rs] % reg[rt];
		break;
	case 25:{//multu
			uint64_t res = reg[rs] * reg[rt];
			cpo_reg[ENTRY_LO] = res & 0XFFFFFFFF;
			cpo_reg[ENTRY_HI] = res >> 32;
		}
		break;
	case 18://mflo
		reg[rd] = cpo_reg[ENTRY_LO];
		break;
	case 16://mfhi
		reg[rd] = cpo_reg[ENTRY_HI];
		break;
	case 17://mthi
		cpo_reg[ENTRY_HI] = reg[rd];
		break;
	case 19://mtlo
		cpo_reg[ENTRY_LO] = reg[rd];
		break;
	default:
		break;
	}//end of switch
}


void HandleIType(reg_type op, reg_type rs, reg_type rt,
				 reg_type immediate){//handle general R type code
	//do not handle jump
	int32_t signed_immed = ((immediate & 0X8000) == 0) ? immediate: 0-((immediate ^ 0XFFFF) + 1);
	switch(op){
	case 35://lw
		reg[rt] = LoadWord(signed_immed + reg[rs]);
		break;
	case 10://slti
		reg[rt] = ((int32_t)reg[rs] < signed_immed);
		break;
	case 11://sltiu
		reg[rt] = (reg[rs] < (word)immediate);
		break;
	case 43://sw
		StoreWord(reg[rt], signed_immed + reg[rs]);
		break;
	case 8://addi
		reg[rt] = reg[rs] + signed_immed;
		break;
	case 9://addiu
		reg[rt] = reg[rs] + immediate;
		break;
	case 12://andi
		reg[rt] = reg[rs] & signed_immed;
		break;
	case 32://lb
		reg[rt] = LoadByte(signed_immed + reg[rs]);
		break;
	case 40://sb
		StoreByte(reg[rt] & 0XFF, signed_immed+reg[rs]);
		break;
	case 15://lui: assign to high 16 bits of reg[rt]
		reg[rt] = immediate << 16;
		break;
	case 13://ori
		reg[rt] = reg[rs] | signed_immed;
		break;
	case 14://xori
		reg[rt] = reg[rs] ^ signed_immed;
		break;
	}
}

int Proceed(address_type *ptr_address){
	int flag = 0;

	if(interrupt()){//jump to int code area
#ifdef PROCEED_ERROR
	puts("Inside interrupt");
#endif		
		HandleException(ptr_address);
		return flag;
	}

	const code_type instruction = LoadWord(*ptr_address);//instruction 32 bit
#ifdef PROCEED_ERROR
	Disassemble(instruction, *ptr_address);
	printf("instruction: %Xh\n", instruction);
#endif
	if(instruction == 0){	//code end with noop instruction
		return END_OF_PROC;
	}
	code_type old_instruction = instruction;
	//------------exec instructions---------------------------
	reg_type op = 0, rs = 0, rd = 0, rt = 0, shamt = 0, func = 0, immediate = 0;//default num

	{
		code_type tmp = instruction;
		op = GetOP(tmp);
		
		tmp >>= (SHAMT_CODE_LEN + FUNC_CODE_LEN);
		size_t pos = REG_CODE_LEN * 2;
		rs = (tmp & (0X1F << pos)) >> pos;
		pos -= REG_CODE_LEN;
		rt = (tmp & (0X1F << pos)) >> pos;
		rd = tmp & 0X1F;
	}//get op rs rt rd

	if(op == 0 || op == 16){//R type
		func = GetFunc(instruction);
		shamt = GetShamt(instruction);
		if(op == 0){
			switch(func){
			case 8://jr
				*ptr_address = reg[rs];
				return flag;
				break;
			case 12://syscall
				SetCause(SYS);
				cpo_reg[EPC] = *ptr_address + 4;
				*ptr_address = EXCEPTION_ADDR;
				SetBit(&cpo_reg[CAUSE], 0XF);//set pending bit
				ClearBit(&cpo_reg[STATUS], EXCEPTION_LEVEL);//disable interrupt
					/*need to rectify perhaps*/
				return flag;
				break;
			default:
				HandleRType(op, rs, rt, rd, shamt, func);
				break;
			}//end of switch
		}//end of if op == 0
		else{
			if(op == 16){
				if(old_instruction == 0X42000018){
					//eret
				ClearBit(&cpo_reg[STATUS], EXCEPTION_LEVEL);
				*ptr_address = cpo_reg[EPC];
				}
				else if(func == 0){
					switch (rs)
					{
					case 4://mtc0
						cpo_reg[rd] = reg[rt];
						break;
					case 0://mfc0
						reg[rt] = cpo_reg[rd];
						break;
					default:
						break;
					}
				}//end of else if
			}//end of if
		}//end of else(op != 0)
	}
	else{//I or J type
		//target = GetTarget(instruction);
		if(op == 2 || op == 3){//j label
			reg_type target = GetTarget(instruction);
			target <<= 2;
			target |= ((*ptr_address + 4) & 0XF0000000);
			if(op == 3){//jal
				reg[31] = *ptr_address + 4;
			}
			*ptr_address = target;
			return flag;
		}
		else{//I type
			immediate = instruction & 0XFFFF;//get immediate
			int difference = immediate;

			if((difference & 0X8000) != 0){//negtive
				difference |= 0XFFFF0000;//extend with sign bit
			}
			size_t label = (int32_t)difference * 4 + *ptr_address + 4;

			switch(op){
			case 4: //beq rs rt label
				if(reg[rs] == reg[rt]){
					*ptr_address = label;
					return flag;
				}
				break;
			case 5://bne
				if(reg[rs] != reg[rt]){
					*ptr_address = label;
					return flag;
				}
				break;
			default:
				HandleIType(op, rs, rt, immediate);
				break;
			}
		}
	}
	*ptr_address += 4;
	return flag;
}


void DisplayReg(void){
	//print reg's info on the screen
	size_t i;
	char RegName[MAX_REG_LEN];
	for(i = 0; i < REG_SIZE; i++){
		GetGPRStr(RegName, i);
		printf("%s=%X ", RegName+1, reg[i]);
		if(((i+1) % 8) == 0)
			putchar('\n');
	}
}