/*
 * pseudo_instr.c
 *
 *  Created on: May 4, 2013
 *      Author: zhaoping
 */
#include "pseudo_instr.h"
#include "..\instruction.h"
#include "..\..\header.h"
#include "..\..\GetStr.h"
#include "asm.h"
#include "label.h"
#include <stdio.h>
#include <string.h>

#define MAX_PSUDO_TRANS_LINE_NUM 10
//max lines of translation of psudo_instruction
#define MAX_PSUDO_INSTR_NUM 20
//max number of pseudo code
struct pseudo_instr_table{
	char pseudoInstr[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN];//operation and other fields
	size_t transLineNum;
	char instr[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN][MAX_PSUDO_TRANS_LINE_NUM];//the value in decimal or define the format
}pseudoInstrTable[MAX_PSUDO_INSTR_NUM];
size_t pseudoInstrNum;

//#define PSEUO_READ_BUG
//#define PSEUDO_BUG
void ReadPseudoInstr(void){
	//read in instructions to global table
	memset(pseudoInstrTable, 0, sizeof(pseudoInstrTable));
	pseudoInstrNum = 0;
	FILE* pFile;
	if((pFile = fopen("pseudo_instr.txt", "r")) == NULL)
		ERROR("Cannot open the pseudo_instr.txt file\n");

	size_t i = 0, j = 0;
	char str[MAX_INSTR_STR_LEN];
	char *line;
	while(fgets(str, MAX_INSTR_STR_LEN, pFile) != NULL){//get in string
		//for every line
#ifdef PSEUO_READ_BUG
		printf("str: %s\n", str);
#endif
		line = str;
		char *oldLinePtr = line;

		char tmpStr[MAX_INSTR_STR_LEN];
		if((line = GetStr(tmpStr, line)) != oldLinePtr){
			sscanf(tmpStr, "%d", &pseudoInstrTable[i].transLineNum);//get num
#ifdef PSEUO_READ_BUG
			printf("transLineNum: %d\n", pseudoInstrTable[i].transLineNum);
#endif
		}
		for(j = 0; (line = GetStr(pseudoInstrTable[i].pseudoInstr[j], line)) != oldLinePtr; j++){
			oldLinePtr = line;
#ifdef PSEUO_READ_BUG
			printf("pseudoInstrTable[%d].pseudoInstr[%d]: %s\n", i, j, pseudoInstrTable[i].pseudoInstr[j]);
#endif
		}

		size_t k = 0;
		for(k = 0; k < pseudoInstrTable[i].transLineNum; k++){//read in translate instruction
			if(fgets(str, MAX_INSTR_STR_LEN, pFile) == NULL) ERROR("Wrong Pseudo Grammar!");
			line = str;
			for(j = 0; (line = GetStr(pseudoInstrTable[i].instr[k][j], line)) != oldLinePtr; j++){
				oldLinePtr = line;
#ifdef PSEUO_READ_BUG
			printf("pseudoInstrTable[%d].instr[%d][%d]: %s\n", i, k, j, pseudoInstrTable[i].instr[k][j]);
#endif
			}
#ifdef PSEUO_READ_BUG
			putchar('\n');
#endif
		}
		pseudoInstrNum++;
		i++;
	}
	fclose(pFile);
}

enum field_type{rs, rt, rd, immediate, label};
void TransPseudo(char *str_op, char *command, pc_type *ptr_pc){
	//if psedo then translate else assemble
	void AnalyzePseudoInstr(size_t index, char *command, char str[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN]);
	void BuildInstructionFromPseudo(size_t index, size_t instrIndex,char *instruction,
			char str[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN]);
	size_t i = 0;
#ifdef PSEUDO_BUG
	printf("str_op: %s\n", str_op);
	printf("pseudoInstrNum: %d\n", pseudoInstrNum);
#endif
	for(i = 0; i < pseudoInstrNum; i++){
#ifdef PSEUDO_BUG
	printf("pseudoInstrTable[%d].pseudoInstr[0]: %s\n", i, pseudoInstrTable[i].pseudoInstr[0]);
#endif
		if(strcmp(pseudoInstrTable[i].pseudoInstr[0], str_op) == 0){
			break;
		}
	}
	if(i == pseudoInstrNum){//not a pseudo_instruction
		AssembleCode(str_op, command, *ptr_pc);
		*ptr_pc += 4;
		return;
	}

	size_t index = i;
	//is a pseudoInstruction
	char str[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN];
	memset(str, 0, sizeof(str));

	AnalyzePseudoInstr(index, command, str);
	//extract element into str
#ifdef PSEUDO_BUG
	printf("After Analyze:  str[immediate]: %s\n", str[immediate]);
#endif		
#ifdef PSEUDO_BUG
	printf("str[rs]: %s str[rd]: %s str[rt]: %s str[immediate]: %s str[label]: %s\n",
			str[rs], str[rd], str[rt], str[immediate], str[label]);
#endif

	for(i = 0; i < pseudoInstrTable[index].transLineNum; i++){
		//for every translated instruction
		char instruction[MAX_INSTR_STR_LEN] = "";//empty
#ifdef PSEUDO_BUG
	puts("Before BuildInstructionFromPseudo");
#endif
		BuildInstructionFromPseudo(index, i, instruction, str);
#ifdef PSEUDO_BUG
	printf("final instruction: %s %s\n",
			pseudoInstrTable[index].instr[i][0], instruction);
#endif
		AssembleCode(pseudoInstrTable[index].instr[i][0], instruction, *ptr_pc);
		*ptr_pc += 4;
	}//end of for every translated instruction
}

void AnalyzePseudoInstr(size_t index, char *command, char str[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN]){
	//extract element and place into the str array
	char line[MAX_INSTR_FIELD_STR_LEN];
	char *oldCommand = command;
	size_t i = 0;
	for(i = 1; *pseudoInstrTable[index].pseudoInstr[i] != 0; i++){
		//for every field of pseudo_instruction
		if((command = GetStr(line, command)) == oldCommand)//extract a string
			ERROR("Wrong Pseudo instruction syntax");
		//do not have corresponding field
#ifdef PSEUDO_BUG
	puts("After getStr");
#endif
		char *grammar = pseudoInstrTable[index].pseudoInstr[i];
		if(strcmp(grammar, "rs") == 0){
			strcpy(str[rs], line);
		}
		else if(strcmp(grammar, "rt") == 0){
			strcpy(str[rt], line);
		}
		if(strcmp(grammar, "rd") == 0){
			strcpy(str[rd], line);
		}
		else if(strcmp(grammar, "immediate") == 0){
			strcpy(str[immediate], line);
#ifdef PSEUDO_BUG
	puts("inside analyze: immediate:");
	printf("immediate: %s\n", immediate);
#endif		
		}
		else if(strcmp(grammar, "label") == 0){
			strcpy(str[label], line);
			reg_type immed;
			if(FindLabel(line, &immed)){
				sprintf(str[immediate], "%d", immed);
#ifdef PSEUDO_BUG
	printf("str[immediate]: %s\n", str[immediate]);
#endif		
			}
		}
		//place element to
	}//end of for every field of pseudo_instruction
}

void BuildInstructionFromPseudo(size_t index, size_t instrIndex,char *instruction,
		char str[MAX_INSTR_FIELD_NUM][MAX_INSTR_FIELD_STR_LEN]){

	size_t j = 0;
	for(j = 1; *pseudoInstrTable[index].instr[instrIndex][j] != 0; j++){
		//for every field of the instruction
		char *instr_grammar = pseudoInstrTable[index].instr[instrIndex][j];
		if(j != 1){
			strcat(instruction, " ");
		}
		if(strcmp(instr_grammar, "rs") == 0){
			strcat(instruction, str[rs]);
		}
		else if(strcmp(instr_grammar, "rd") == 0){
			strcat(instruction, str[rd]);
		}
		else if(strcmp(instr_grammar, "rt") == 0){
			strcat(instruction, str[rt]);
		}
		else if(strcmp(instr_grammar, "immediate") == 0){
			strcat(instruction, str[immediate]);
		}
		else if(strcmp(instr_grammar, "label") == 0){
			strcat(instruction, str[label]);
		}
		else{//none matches
			strcat(instruction, instr_grammar);
		}
#ifdef PSEUDO_BUG
	printf("instr_grammar: %s, instruction: %s\n", instr_grammar, instruction);
#endif
	}//end of for every field of the instruction
}
