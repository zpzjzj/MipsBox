/*
 * AnalyzeCmd.c
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */
#include <ctype.h>
#include <string.h>

#include "Assemble.h"
#include "asm.h"
#include "label.h"
#include "..\..\header.h"
#include "pseudo_instr.h"
#include "..\..\assembleInit.h"

//#define ASM_ERROR
//#define ANALYZE_ERROR
//#define CMD_ERROR

int Analyze(char *command, pc_type *ptr_pc);//analyze each line of asm code
void Assemble(pc_type pc_start){
	//top for assemble
	//assemble lines of code from stdin
	void HandleDataSeg(char *line, pc_type *ptr_pc);
	Init();
	InitLabel();
	pc_type asm_pc = pc_start;
	char line[MAX_STR_LEN];
	
	while(1){
		printf("%X:", asm_pc);//print pc address
		gets(line);
#ifdef ASM_ERROR
		printf("Assemble command: %s\n", line);
#endif
		if(strcmp(line, ".data") == 0){
			GetLabel("data: ", asm_pc);//add label
			HandleDataSeg(line, &asm_pc);
		}
#ifdef ASM_ERROR
		puts("out of dataSeg() back to assemble()");
		printf("line: %s\n", line);
#endif		
		if(strcmp(line, ".text") == 0){
#ifdef ASM_ERROR
		puts("inside .text");
#endif		
			GetLabel("text: ", asm_pc);
			continue;
		}
		if(Analyze(line, &asm_pc) == QUIT){//analyze each line
			break;
		}
	}
#ifdef ASM_ERROR
	puts("before ModifyLabel");
#endif
	ModifyLabel();
	//find label and replace
}

enum storeFormat{end, nEnd};
//end: save directly into code and align with 0
//nEnd: store for a whole word and write
void HandleDataSeg(char *line, pc_type *ptr_pc){
	void StoreAscii(char *line, pc_type *ptr_pc);
	void EatInterpunction(char *str);

#ifdef ASM_ERROR
		puts("inside HandleDataSeg");
#endif
	{//add j TEXT at the beginning
		char str[MAX_STR_LEN] = "j TEXT";
		Analyze(str, ptr_pc);
	}
	printf("%d:", *ptr_pc);
	gets(line);
	char *lineAddr = line;
	char *old_line = line;
	const size_t MAX_OP_LEN = 15;
	char str[MAX_OP_LEN];

	while((line = GetLabel(line, *ptr_pc)) != old_line){
		//until data definition ends

		line = GetStr(str, line);
		EatInterpunction(str);
		if(strcmp(str, ".asciiz") == 0){
			//asciiz type
			StoreAscii(line, ptr_pc);
		}

		line = lineAddr;
		printf("%d:", *ptr_pc);
		gets(line);
#ifdef ASM_ERROR
		printf("line: %s\n", line);
#endif
		old_line = line;
	}
}
void StoreAscii(char *line, pc_type *ptr_pc){
	void StoreChar(char ch, pc_type *ptr_pc, storeFormat type);

	char ch;
	while((ch = *line) != '\0'){//until the end of line
		while((ch = *line) == ' ') line++;//skip space
		if(ch == '\''){//met a char
			line++;//next c
			StoreChar(*line, ptr_pc, nEnd);
			line+=2;//skip '\''
			ch = *line;
			//suppose 'c' format
		}
		if(ch == '\"'){//met a string
			//suppose no " \" " format

			line++;
			while((ch = *line) != '\"'){
				StoreChar(ch, ptr_pc, nEnd);
				line++;
			}
			StoreChar('\0', ptr_pc, nEnd);
			line++; //skip '\"'
		}
	}
	StoreChar('\0', ptr_pc, end);//write all to the data
}
void StoreChar(char ch, pc_type *ptr_pc, storeFormat type){
	//store with little endian
#ifdef ASM_ERROR
		puts("inside StoreChar()");
		printf("ch: %c\n", ch);
#endif
	static uint32_t data = 0;
	static size_t count = 0; //0 ~ 3
	uint32_t tmp = ch;
	tmp <<= count * 8;
	switch (type){
	case nEnd:	
		data |= tmp;
		count++;
		if(count != 4) break;
	case end://count == 4 or end
		AddCommandCode(*ptr_pc, data, D);//data type ._.
#ifdef ASM_ERROR
		printf("data: %X\n", data);
#endif
		*ptr_pc += 4;
		count = 0;
		data = 0;
	}
}
int Analyze(char *command, pc_type *ptr_pc){
	//analyze each line of asm code
	//deal with empty line and label
	//here "quit" for end
	void EatInterpunction(char *str);
	const size_t MAX_OP_LEN = 15;
	char str_op[MAX_OP_LEN];

	int flag = 0;
	EatInterpunction(command);
	//replace , ( ) with space
	char *oldCommand = command;
	command = GetLabel(command, *ptr_pc);
	if(command != oldCommand)//if has a label
		flag = 1;
#ifdef ASM_ERROR
		printf("flag: %d\n", flag);
#endif
	{//check for empty line and get op_str
		char *oldCommand = command;
		command = GetStr(str_op, command);
		if(oldCommand == command && flag == 0){
			//do not get any instruction-> empty line
#ifdef ASM_ERROR
		printf("Empty line\n");
#endif
			return 0;
		}
		else if(oldCommand == command && flag == 1){
			//empty but has a label
			return 0;
		}//can be combined with the last one
		else if(strcmp(str_op, "quit") == 0){
#ifdef ASM_ERROR
		printf("QUIT: str_op: %s\n", str_op);
#endif
			return QUIT;
		}
	}
	TransPseudo(str_op, command, ptr_pc);
	return 0;
}

void EatInterpunction(char *str){
	//used to handle the command at very first
	//replace comma and parentheses with space
	int flag = 0;//flag for brackets match
	while(*str != '\0' && *str != '#'){//# comment
		if(*str == ',') *str = ' ';
		else if(*str == '(' || *str == ')'){
			//not check for ")...("
			flag = !flag;
			*str = ' ';
		}
		else
			*str = tolower(*str);
		str++;
	}
	*str = '\0';//end comment
	if(flag != 0)
		ERROR("Input syntax error");
}

static struct commandcode{
	//code that transferred from instruction
	pc_type pc;
	instr_type type;//r or i or j
	code_type code;
}codeTable[MAX_LINE_NUM/4];//for every pc
static size_t commandNum= 0;

void AddCommandCode(pc_type pc, code_type code, instr_type type){
	//add the code transferred from command
	codeTable[commandNum].pc = pc;
	codeTable[commandNum].type = type;
	codeTable[commandNum].code = code;
#ifdef CMD_ERROR
		printf("AddCommandCode: code: %X commandNum: %d\n", codeTable[commandNum].code, commandNum);
#endif
	commandNum++;
}

enum{PC_NOT_FOUND = -1};
int GetIndexByPC(pc_type pc){//for codeTable
	for(size_t i = 0; i < commandNum; i++){
			if(codeTable[i].pc == pc)
				return i;
	}
	return PC_NOT_FOUND;
//undefined if cannot find
}
code_type GetCommandCode(pc_type pc){
		size_t index = GetIndexByPC(pc);
		return codeTable[index].code;
}

bool WriteCommandCode(code_type code, pc_type pc){
#ifdef CMD_ERROR
		printf("inside WriteCmdCode() pc: %d code: %X\n", pc, code);
#endif
		int index = GetIndexByPC(pc);
		if(index == PC_NOT_FOUND)
			return false;
		else{
			codeTable[index].code = code;
			return true;
		}
}

void PrintCommandCode(void){
	//print all the code
	void PrintCode(instr_type type, code_type code);
	size_t i;
	for(i = 0; i < commandNum; i++){
#ifdef CMD_ERROR
		printf("inside Print: code: %X\n", codeTable[i].code);
#endif
		switch(codeTable[i].type){
		case R:
		case I:
		case J:
		case D:
			printf("%X: ", codeTable[i].pc);
			PrintCode(codeTable[i].type, codeTable[i].code);
			break;
		default:
			ERROR("Wrong type\n");
			break;
		}
	}
}

void WriteCodeToFile(char *fileName){
	FILE *pFile = fopen(fileName, "wb");
	if(pFile == NULL)
		ERROR("Cannot open the file to write.");
	size_t i;
	for(i = 0; i < commandNum; i++){
#ifdef CMD_ERROR
		printf("WriteCodeToFile: codeTable[%d].code: %X\n", i, codeTable[i].code);
#endif
		fwrite(&codeTable[i].code, sizeof(code_type), 1, pFile);
	}
}

void PrintCode(instr_type type, code_type code){
	const int EMPTY = -1;
	int seperate_pos[5];
	{//set seperate_pos
		const size_t OPCODE_LEN = 6;
		const size_t REG_LEN = 5;
		const size_t SHAMT_LEN = 5;
		const size_t FUNC_LEN = 6;

		memset(seperate_pos, EMPTY, sizeof(seperate_pos));
		switch(type){
		case R:
		case I:
			seperate_pos[1] = OPCODE_LEN+REG_LEN;
			seperate_pos[2] = seperate_pos[1] + REG_LEN;
		case J:
			seperate_pos[0] = OPCODE_LEN;
			break;
		case D:
			break;
		}
		
		if(type == R){
			seperate_pos[3] = seperate_pos[2] + REG_LEN;
			seperate_pos[4] = seperate_pos[3] + SHAMT_LEN;
		}
	}//deside seperate_position
	
	size_t i = 0, j = 0;
	code_type mask = 0X80000000;
	for( ; i < 32; i++, mask>>=1){
		if(i == seperate_pos[j]){
			putchar(' ');
			if(seperate_pos[j] != EMPTY)
				j++;
		}
		putchar('0'+ ((mask&code) != 0));
	}
	putchar('\n');
}
