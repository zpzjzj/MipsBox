#include "display.h"
#include "memory.h"
#include "reg.h"

#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

HANDLE  hConsoleOut;                 // Handle to the console 
HANDLE  hRunMutex;                   // "Keep Running" mutex 
HANDLE  hScreenMutex;                // "Screen update" mutex
int     ThreadNr;                    // Number of threads started 
CONSOLE_SCREEN_BUFFER_INFO csbiInfo; // Console information 

extern bool display_finished;
extern bool prog_finished;

void CreateDispThd(void) // Dispatch threads.
{
	void Display(void *argv);

    WriteTitle();

	ClearScreen();
	void *ArgList = NULL;
	_beginthread(Display, 2048, ArgList);
	ThreadNr++;
}

const size_t MAX_Y = 60;
void Display(void *argv)
{//display memory
    char    MyCell, OldCell;
    WORD    MyAttrib, OldAttrib;
    char    BlankCell = 0x20;//' '
	COORD   Coords = {0,0}, Delta;
    COORD   Old = {0,0};
    DWORD   Dummy;

	address_type disp_addr = MEMORY_DISP_ADDR;

    // Generate update increments and initial 
    // display coordinates.
    MyAttrib = 0x0F;   // force black background 
	size_t count = 0;
	ClearScreen();
	const size_t TOTAL_SIZE = csbiInfo.dwSize.X * MAX_Y / PAGE_SIZE + 1;//! floor num
	
	bool last_scanf_flag = 0;	//if program ends then refresh for last time
    do{//print every char
		Coords.X = Coords.Y = 0;
		for(size_t i = 0; i < TOTAL_SIZE + 1; i++){//for every part
			//get every page's first addr
			byte *mem_ptr = GetPageBlock(disp_addr+i*PAGE_SIZE);
			for(size_t j = 0; j < PAGE_SIZE + 1; j++){//for every byte in the block
				MyCell = mem_ptr[j];	//get every byte
				WriteConsoleOutputCharacter(hConsoleOut, &MyCell, 1, Coords, &Dummy);
				WriteConsoleOutputAttribute(hConsoleOut, &MyAttrib, 1, Coords, &Dummy);
				// Increment the coordinates for next placement of the block. 
				Coords.X += 1;
				if(Coords.X >= csbiInfo.dwSize.X){
					Coords.X = 0;
					Coords.Y += 1;
				}
		
				if(Coords.Y >= MAX_Y){
					if(last_scanf_flag){
						display_finished = true;
						while(1) ;//wait
					}	
					if(prog_finished){//end
						last_scanf_flag = true;
					}
					Coords.X = 0;
					Coords.Y = 0;//again
					i = TOTAL_SIZE;
					j = PAGE_SIZE;
					//break out
				}
    		}//end of for every byte
		}//end of for every block
		ReleaseMutex(hRunMutex);//unlock 
	}//end of while
    while (WaitForSingleObject(hRunMutex, 75L) == WAIT_TIMEOUT);
	// Repeat while RunMutex is still taken. 
}

void CreateThd(void (*func_ptr)(void*)) // Dispatch threads.
{
	void *ArgList = NULL;
	_beginthread(func_ptr, 2048, ArgList);
	ThreadNr++;
}

void CountTime(void *argv){
	time_t currTime;
	while(WaitForSingleObject(hRunMutex, 75L) == WAIT_TIMEOUT){
		currTime = time(&currTime);
		//printf("currTime: %X\n", currTime);
		WaitForSingleObject(hScreenMutex, INFINITE);

		size_t tmp = currTime & 0XFFFFFFFF;
		StoreWord(tmp, SYS_TIME_ADDR);
		tmp = currTime >> 32;
		StoreWord(tmp, SYS_TIME_ADDR+4);
		ReleaseMutex(hScreenMutex);
		ReleaseMutex(hRunMutex);
	}
}

void IOKey(void *argv){
	reg_type receiver_control = LoadWord(RECEIVER_CONTROL);
	SetBit(&receiver_control, IO_INTERRUPT_ENABLE);
	while(!prog_finished && (WaitForSingleObject(hRunMutex, 75L) == WAIT_TIMEOUT)){
		if(kbhit()){
			char ch = getch();
		/*	for(size_t i = 0 ; i < 1000; i++)
				printf("Inside if kbhit() ch = %c\n", ch);*/
			StoreByte(ch, RECEIVER_DATA);
			
			//muxtex in StoreByte function
			if(bitState(cpo_reg[STATUS], INTERRUPT_ENABLE) && bitState(cpo_reg[STATUS], EXCEPTION_LEVEL)
				&& bitState(receiver_control, IO_INTERRUPT_ENABLE)){
				SetCause(INTERRUPT);
				SetBit(&cpo_reg[CAUSE], 0XD);//set pending bit
				/*need to rectify perhaps*/
				SetBit(&receiver_control, IO_READY);
				StoreWord(receiver_control, RECEIVER_CONTROL);
			/*	for(size_t i = 0 ; i < 1000; i++)
					printf("Inside inside of kbhit() set state cause: %X\n", cpo_reg[CAUSE]);*/
			}//end of if
		}//end of if kbhit
		ReleaseMutex(hRunMutex);//unlock
	}//end of while prog_finished
}

void WriteTitle(void){//the title of the window
    char Msg[] = "MIPS";
    SetConsoleTitle(Msg);
}

void ClearScreen(void){
	address_type base_addr = MEMORY_DISP_ADDR;
	size_t total_size = MAX_Y * csbiInfo.dwSize.X;
	for(size_t i = 0; i < total_size; i++){
		StoreByte(0, base_addr + i);
	}
}

void RefreshScreen(void)
{
    DWORD    dummy;
    COORD    Home = {0, 0};
    FillConsoleOutputCharacter(hConsoleOut, ' ', 
      csbiInfo.dwSize.X * csbiInfo.dwSize.Y, 
      Home, &dummy);
}