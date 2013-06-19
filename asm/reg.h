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

extern char* GetRegFieldStr(char *command, reg_type *ptr_reg);//get reg str from command
extern void GetRegStr(char *strReg, reg_type reg);//get reg str from code
extern void InitRegTable(void);
extern void DestroyRegTable(void);
typedef uint32_t reg_type;
#endif /* REG_H_ */
