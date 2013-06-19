/*
 * reg.h
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */

#ifndef REG_H_
#define REG_H_

#include "codeType.h"
#include "memory.h"

#define REG_SIZE 32
#define COPROCESSOR_REG_SIZE 15
typedef uint32_t reg_type;

enum reg_name{RA = 31, SP = 29};
enum reg_bit{USER_MODE = 4, EXCEPTION_LEVEL = 1, INTERRUPT_ENABLE = 0, BRANCH_DELAG = 31};
enum exception_cause{INTERRUPT = 0, SYS = 8};
enum copro_reg{INDEX=0, RANDOM=1, ENTRY_LO=2, ENTRY_HI=10, 
	CONTEXT_REG=4, BAD_VADDR=8, STATUS=12, CAUSE=13, EPC=14};
enum IO_reg{RECEIVER_CONTROL= 0XFFF00, RECEIVER_DATA = 0XFFF04, 
	TRANSMITTER_CONTROL = 0XFFF08, TRANSMITTER_DATA = 0XFFF0C};
enum IO_reg_bit{IO_READY=0, IO_INTERRUPT_ENABLE=1};


extern word reg[REG_SIZE];//register array
extern word cpo_reg[COPROCESSOR_REG_SIZE];

extern void SetCause(size_t cause);
extern void reg_init(void);
extern bool interrupt(void);

extern bool bitState(reg_type reg, size_t index);
extern void SetBit(reg_type* reg_ptr, size_t index);
extern void ClearBit(reg_type* reg_ptr, size_t index);
//bit manipulate for register
#endif /* REG_H_ */
