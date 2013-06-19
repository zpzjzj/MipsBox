#include "TransCode\dasm\dasm_Disassemble.h"
#include "dasm_codeType.h"
#include "dasm_header.h"
#include <stdio.h>
#include <string.h>

code_type code[MAX_LINE_NUM];


int main(void){
	puts("Input the .com name");
	char fileName[30];
	gets(fileName);
	FILE *fp = fopen(fileName, "rb");
	if(fp == NULL)
		ERROR("Cannot Open the .com file");
	memset(code, 0, sizeof(code));
	size_t count = fread(code, sizeof(code_type), MAX_LINE_NUM, fp);

	size_t pc_start;
	puts("Input the start pc(hex)");
	scanf("%X", &pc_start);
	getchar();
	Disassemble(code, count, pc_start);
	return 0;
}