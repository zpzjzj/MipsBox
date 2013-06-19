#include "display.h"
#include "memory.h"
#include "reg.h"

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

void CreateKeyboardThd(void){
	void IOKey(void *argv);
	void *ArgList = NULL;
	_beginthread(IOKey, 2048, ArgList);
	ThreadNr++;
}

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
	const size_t MAX_Y = 36;
	const size_t TOTAL_SIZE = csbiInfo.dwSize.X * MAX_Y / PAGE_SIZE + 1;//! floor num
	
	bool last_scanf_flag = 0;	//if program ends then refresh for last time
    do{//print every char
		Coords.X = Coords.Y = 0;
		for(size_t i = 0; i < TOTAL_SIZE + 1; i++){//for every part
			WaitForSingleObject(hScreenMutex, INFINITE);
			address_type addr = GetTmpAddr(disp_addr+i*PAGE_SIZE);
			//get every page's first addr
			for(size_t j = 0; j < PAGE_SIZE + 1; j++){//for every byte in the block
				MyCell = memory[addr+j];	//get every byte
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
			ReleaseMutex(hScreenMutex);//unlock
		}//end of for every block
		ReleaseMutex(hRunMutex);//unlock
	}//end of while
    while (WaitForSingleObject(hRunMutex, 75L) == WAIT_TIMEOUT);
	// Repeat while RunMutex is still taken. 
}

void IOKey(void *argv){
	while(!prog_finished && (WaitForSingleObject(hRunMutex, 75L) == WAIT_TIMEOUT)){
		if(kbhit()){
			char ch = getch();
			StoreByte(ch, RECEIVER_DATA);
			reg_type receiver_control = LoadWord(RECEIVER_CONTROL);
			//muxtex in StoreByte function
			if(bitState(cpo_reg[STATUS], INTERRUPT_ENABLE) && bitState(cpo_reg[STATUS], EXCEPTION_LEVEL)
				&& bitState(receiver_control, IO_INTERRUPT_ENABLE)){
				SetCause(INTERRUPT);
				SetBit(&cpo_reg[STATUS], 0XD);//set pending bit
				/*need to rectify perhaps*/

				SetBit(&receiver_control, IO_READY);
			}//end of if
		}//end of if kbhit
	}//end of while prog_finished
}

void WriteTitle(void){//the title of the window
    char Msg[] = "MIPS";
    SetConsoleTitle(Msg);
}

void ClearScreen(void)//TODO
{
    DWORD    dummy;
    COORD    Home = {0, 0};
    FillConsoleOutputCharacter(hConsoleOut, ' ', 
      csbiInfo.dwSize.X * csbiInfo.dwSize.Y, 
      Home, &dummy);
}