/*
 * pseudo_instr.h
 *
 *  Created on: May 4, 2013
 *      Author: zhaoping
 */

#ifndef PSEUDO_INSTR_H_
#define PSEUDO_INSTR_H_

#include "..\..\codeType.h"
extern void ReadPseudoInstr(void);
extern void TransPseudo(char *str_op, char *command, pc_type *ptr_pc);

#endif /* PSEUDO_INSTR_H_ */
