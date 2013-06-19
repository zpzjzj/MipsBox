/*
 * label.c
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */
#include "label.h"
#include "..\..\header.h"
#include "..\..\reg.h"
#include "..\..\GetStr.h"
#include "Assemble.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LABEL_BUG
#define MAX_LABEL_RECORD_NUM 100
#define MAX_LABEL_TABLE_LINE_NUM 100
struct label_table{//label get
	char label[MAX_LABEL_LEN];
	pc_type address;
}labelTable[MAX_LABEL_TABLE_LINE_NUM];
size_t labelTableNum = 0;

struct label_record{//unsolved label record
	pc_type pc;
	char label[MAX_LABEL_LEN];
	code_type type;
}labelRecord[MAX_LABEL_RECORD_NUM];
size_t labelRecordNum = 0;

void InitLabel(void){
	memset(labelTable, 0, sizeof(labelTable));
	memset(labelRecord, 0, sizeof(labelRecord));
	labelTableNum = 0;
	labelRecordNum = 0;
}

int FindLabel(char *str_field, reg_type *immediate){
	//if find the label immediate = that address and return 1 else return 0
	int flag = 0;
	size_t i = 0;
	for(i = 0; i < labelTableNum; i++){
		if(strcmp(str_field, labelTable[i].label) == 0){
			flag = 1;
			*immediate = labelTable[i].address;
			break;
		}
	}
	return flag;
}

void AddLabelRecord(char *str_field, pc_type pc, code_type type){
	//add unsolved label
	//pc of the instr which cannot find label's addr
	strcpy(labelRecord[labelRecordNum].label, str_field);
	labelRecord[labelRecordNum].pc = pc;
	labelRecord[labelRecordNum].type = type;
	labelRecordNum++;
}

void AddLabelTuple(char *str_label, pc_type pc){
	//add label
	strcpy(labelTable[labelTableNum].label, str_label);
	labelTable[labelTableNum].address = pc;
	labelTableNum++;
}
void ModifyLabel(void){
	//change label with pc address

	size_t i;
	pc_type pc;
	code_type code;
	reg_type immediate;
	for(i = 0; i < labelRecordNum; i++){
		pc = labelRecord[i].pc;//find an unsolved label
		code = GetCommandCode(pc);
		if(FindLabel(labelRecord[i].label, &immediate) == 0){
			printf("label: %s ", labelRecord[i].label);
			ERROR("Can not find\n")
		}
		else if(labelRecord[i].type == I){
			immediate = (int32_t)(immediate - (pc + 4)) >> 2;
			code &= 0XFFFF0000;//clear last 16 bits
			code |= immediate;
		}
		else{
			//suppose only I, J type
#ifdef LABEL_BUG
			printf("label: %s, immediate: %d\n",labelRecord[i].label, immediate);
#endif
			immediate = immediate >> 2;
			code &= 0XFC000000;//clear last 26 bits
			code |= immediate;
		}
		WriteCommandCode(code, pc);
#ifdef LABEL_BUG
			printf("code: %X, pc: %d\n",code, pc);
			printf("actual: %X\n",GetCommandCode(pc));
#endif
	}
}

char *GetLabel(char *command, size_t pc){
	//extract the label from command if exists
	//if get Label then flag got 1 return string that has eat the label
	//if get label then store it in the table
	//label and the instruction must be seperated with a space
	int flag = 0;//flag == 1 means get the label
	char strLabel[MAX_LABEL_LEN];
	//string of label in str
	char *oldCommand = command;
	
	command = GetStr(strLabel, command);//get string

	size_t len = strlen(strLabel);
	if(len == 0)//empty line
		return command;
	if(strLabel[len - 1] == ':'){
		//if end with ':' then eliminate that(LABEL:)
		strLabel[len - 1] = '\0';
		flag = 1;
	}
	else{
		//find the ':' and eat it (LABEL :)
		char strColon[MAX_LABEL_LEN];
		command = GetStr(strColon, command);
		if(strcmp(strColon, ":") == 0){
			//whether is a label else not eat
			flag = 1;
		}
	}

	if(flag == 1){//find the label eat it
		for(size_t i = 0; strLabel[i] != '\0'; i++){
			strLabel[i] = tolower(strLabel[i]);
		}
		AddLabelTuple(strLabel, pc);
		return command;
	}
	else return oldCommand;
}
