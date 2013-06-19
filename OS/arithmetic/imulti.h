/*
 * imulti.h
 *
 *  Created on: Apr 15, 2013
 *      Author: zhaoping
 */

#ifndef IMULTI_H_
#define IMULTI_H_
LOperand_type __imul(operand_type multiplicand, operand_type multiplier);
LOperand_type __idiv(LOperand_type dividend, operand_type divisor, operand_type *ptrRemainder);

#endif /* IMULTI_H_ */
