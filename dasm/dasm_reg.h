/*
 * reg.h
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */

#ifndef REG_H_
#define REG_H_

#include "dasm_codeType.h"

enum reg_field{RS, RT, RD};

extern void GetRegStr(char *strReg, reg_type reg, reg_type op, reg_field field);
//get reg str from command
extern void GetRegStr(char *strReg, reg_type reg);//get reg str from code
extern void InitRegTable(void);
extern void DestroyRegTable(void);
typedef uint32_t reg_type;

#endif /* REG_H_ */
