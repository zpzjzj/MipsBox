/*
 * label.h
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */

#ifndef LABEL_H_
#define LABEL_H_

#include "..\..\codeType.h"
//max label length in the string
#define MAX_LABEL_LEN 15

extern void InitLabel(void);
extern void AddLabelTuple(char *str_label, pc_type pc);
//add label and its address info

extern void AddLabelRecord(char *str_field, pc_type pc, code_type type);
//add unsolved label for modify later

extern int FindLabel(char *str_field, reg_type *immediate);
//find label in the table and get its address
extern void ModifyLabel(void);
char *GetLabel(char *command, size_t pc);
//extract the label from command if exists

#endif /* LABEL_H_ */
