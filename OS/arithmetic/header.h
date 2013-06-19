/*
 * header.h
 *
 *  Created on: Mar 22, 2013
 *      Author: zhaoping
 */

#ifndef HEADER_H_
#define HEADER_H_
/*headfile for interger arithmetic*/
#include <stdint.h>
#include <stdio.h>

typedef uint32_t operand_type;
typedef uint64_t LOperand_type;
typedef uint32_t float_type;
typedef uint64_t double_type;

#include "floatOperation.h"
#include "fsTrans.h"

#define operand_size 32
#define MAX_STR_LEN  80
#define MAXSTRLEN 30
#define PrintOF(of);  if(of) printf("Add operation overflowed\n");
#define PrintCF(cf);  if(cf) printf("Add operation has a carry\n");

#endif /* HEADER_H_ */
