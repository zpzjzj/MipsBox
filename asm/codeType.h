/*
 * codeType.h
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */

#ifndef CODETYPE_H_
#define CODETYPE_H_

#include <stdint.h>
#include <stdlib.h>
typedef unsigned int reg_type;
typedef	enum type{R, I, J, D} instr_type;//type of an instruction
typedef uint32_t code_type;
typedef size_t pc_type;
typedef uint64_t word;
typedef uint32_t address_type;

#endif /* CODETYPE_H_ */
