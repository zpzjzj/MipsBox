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
typedef uint32_t reg_type;
typedef	enum type{R, I, J} instr_type;//type of an instruction
typedef uint32_t code_type;
typedef size_t pc_type;

typedef uint8_t byte;
typedef uint16_t half_word;
typedef uint32_t word;

#endif /* CODETYPE_H_ */
