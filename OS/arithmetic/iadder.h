/*
 * iadder.h
 *
 *  Created on: Apr 14, 2013
 *      Author: zhaoping
 */

#ifndef IADDER_H_
#define IADDER_H_

#include "header.h"

extern operand_type *ptrOF;
extern operand_type *ptrCF;
operand_type trans_compl(operand_type num);
LOperand_type trans_compl_64bit(LOperand_type num);
LOperand_type __atoi(char *s);
char * __itoa(operand_type num, char *str_num);
char * __itoa_64bit(LOperand_type num, char *str_num);
operand_type __iadd(operand_type op1, operand_type op2, operand_type *ptrCF, operand_type *ptrOF);
LOperand_type __iadd_64bit(LOperand_type op1, LOperand_type op2, operand_type *ptrCF, operand_type *ptrOF);
operand_type __isub(operand_type op1, operand_type op2, operand_type *ptrCF, operand_type *ptrOF); //op1 - op2
LOperand_type __isub_64bit(LOperand_type op1, LOperand_type op2, operand_type *ptrCF, operand_type *ptrOF); //op1 - op2
#endif /* IADDER_H_ */
