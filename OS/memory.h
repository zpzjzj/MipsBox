/*
 * memory.h
 *
 *  Created on: Apr 17, 2013
 *      Author: zhaoping
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>
#include <stdio.h>
#include "codeType.h"

typedef size_t page_type;

#define PAGE_SIZE 0X400
//1K
#define MEMORY_DISP_ADDR 0XB8000
#define MEMORY_CURSOR_ADDR 0XB7FFC
//for mips code usage
#define STACK_ADDR	0XFFFFF
#define KEY_BUFF_ADDR	0XD0000
//for mips code usage
#define SYS_TIME_ADDR 0XD8000
#define EXCEPTION_ADDR 0X80180
//original 0X80000180

#define TOTAL_VISITABLE_SIZE 1024*1024 //1M => 1024K
//0X100000
#define MEMORY_SIZE 0X1000 //4K
#define CODE_SIZE 0X40000	//256K
#define MAX_REG_LEN 10
typedef uint32_t address_type;


extern byte memory[MEMORY_SIZE];
extern pc_type gl_pc;
extern void StoreWord(word data, address_type address);
extern word LoadWord(address_type address);
extern void StoreHalfWord(half_word data, address_type address);
extern half_word LoadHalfWord(address_type address);
extern void StoreByte(byte data, address_type address);
extern byte LoadByte(address_type address);

extern address_type GetTmpAddr(address_type addr);
//need lock
extern void memory_init(void);
extern void DisplayData(address_type);
extern size_t memory_fread(address_type addr, size_t size, size_t count, FILE* fp);
extern byte *GetPageBlock(address_type start_addr);

#endif /* MEMORY_H_ */
