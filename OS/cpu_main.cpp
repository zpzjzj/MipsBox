#include <windows.h>
#include <conio.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "reg.h"
#include "header.h"
#include "proceed.h"
#include "display.h"

extern void DisplayReg(void);

bool display_finished = 0;
bool prog_finished = 0;

//#define MAIN_RUN_ERROR
//#define MAIN_ERROR
//#define MAIN_DISP_ERROR

const bool EXIT = true;
int main(void){
	void InitCPU(void);
	void LoadCPU(void);
	bool AnalyzeCmd(char *cmd);
	/*	printf("csbiInfo.dwSize.X: %X, csbiInfo.dwSize.Y: %X\n", 
		csbiInfo.dwSize.X, csbiInfo.dwSize.Y);*/
	const size_t MAX_CMD_LEN = 100;
	char command[MAX_CMD_LEN];
	while(1){
		hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);
		hScreenMutex = CreateMutex(NULL, FALSE, NULL);  // Cleared 
		hRunMutex = CreateMutex(NULL, TRUE, NULL);      // Set 
		ThreadNr = 0;
	
		display_finished = 0;
		prog_finished = 0;//clear
		InitCPU();
		LoadCPU();
		printf("mips> ");
		gets(command);
		CreateDispThd();
		CreateThd(IOKey);
		CreateThd(CountTime);
		
		display_finished = false;
		prog_finished = false;

		if(AnalyzeCmd(command) == false){
			prog_finished = true;
			while(!display_finished) ;//wait for display end
		}
		else//EXIT
			break;
		ShutDown();
		CloseHandle(hScreenMutex);
		CloseHandle(hRunMutex);
		// All threads done. Clean up handles.
	}
	CloseHandle(hConsoleOut);
}

void ShutDown(void) // Shut down threads 
{
    while (ThreadNr > 0){
        // Tell thread to die and record its death.
        ReleaseMutex(hRunMutex);
        ThreadNr--;   
    }
}

void InitCPU(void){//init CPU data
	//register PC
	memory_init();
	reg_init();
	gl_pc = 0;
	memset(reg, 0, sizeof(reg));
	reg[29] = STACK_ADDR;
}

size_t LoadProgram(const size_t MAX_COM_SIZE, FILE* fp, pc_type pc);
void LoadCPU(void){//load system program
	FILE *fp = fopen("sys.com", "rb");
	if(fp == NULL){
		ERROR("Cannot find the program");
	}
	const size_t MAX_COM_SIZE = 65536;//align to page size 1024 (1K)
	//64K
	LoadProgram(MAX_COM_SIZE, fp, EXCEPTION_ADDR);
	fclose(fp);
}

bool AnalyzeCmd(char *cmd){//cmd end with exit
	if(strcmp(cmd, "exit") == 0)
		return EXIT;
	else if(strcmp(cmd, "") == 0)
		return false;
	else if(strcmp(cmd, "refresh") == 0){
		ClearScreen();
		return false;
	}

	FILE *fp = fopen(cmd, "rb");
	if(fp == NULL){
		ERROR("Cannot find the program");
	}
	const size_t MAX_COM_SIZE = 512;
	size_t ElementNumber = LoadProgram(512, fp, gl_pc);

	size_t count = 0;
	for(size_t i = 0; ; i++){
		if(Proceed(&gl_pc) == END_OF_PROC){
#ifdef MAIN_RUN_ERROR
			puts("Proc ended");
#endif
			break;
		}
#ifdef MAIN_RUN_ERROR
		printf("%dth proceed------------\n", i);
		{
			DisplayReg();
			char ch;
			size_t cnt = reg[SP]; 
			puts("sp:");
			do{
				ch = LoadByte(cnt);
				printf("%d(%c) ", ch, ch);
				cnt++;
			}while(cnt < 12+reg[29]);
			puts("\nafter print stack");
		}
		if(gl_pc >= EXCEPTION_ADDR)
		{ //debug sys code
			char ch = getchar();
			if(ch == 'd'){
				DisplayData(MEMORY_DISP_ADDR);
			}
			if(ch != '\n') getchar();//eat endl
		}
#endif
	}
#ifdef MAIN_ERROR
	puts("Program end");
#endif
	return false;
}
size_t LoadProgram(const size_t MAX_COM_SIZE, FILE* fp, pc_type pc){
	size_t size = memory_fread(pc,sizeof(byte), MAX_COM_SIZE, fp);
	fclose(fp);
	return size;
}
