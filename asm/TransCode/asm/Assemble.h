/*
 * AnalyzeCmd.h
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */

#ifndef ANALYZECMD_H_
#define ANALYZECMD_H_

#include <stdio.h>
#include "..\..\codeType.h"
#define QUIT 1
#define MAX_LINE_NUM 1024*1024//max lines of command

extern char *GetStr(char *dStr, char *sStr);
extern void Assemble(pc_type pc_start);
extern void AddCommandCode(pc_type pc, code_type code, instr_type type);
extern void PrintCommandCode(void);//print code on the screen
void WriteCodeToFile(char *fileName);
extern bool WriteCommandCode(code_type code, pc_type pc);
extern code_type GetCommandCode(pc_type pc);

#endif /* ANALYZECMD_H_ */
