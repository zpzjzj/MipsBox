/*
 * reg.c
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */
#include "reg.h"
#include <conio.h>
#include <process.h>
#include <windows.h>

reg_type reg[REG_SIZE] = {0};//store register
//$zero should always be 0

reg_type cpo_reg[COPROCESSOR_REG_SIZE];
//coprocessor_reg

/*
BR: branch
Note that whenever an interrupt is serviced by software, 
the pending bit of the interrupt pending filed should be cleared
EXC: Exception Code. (store int number(or which bit))
When any exception happens, the exception code will be written into the Cause(#13) system register. 
And software can use the information provided by Cause register to determine which ISR to invoke.*/
/*
When any exception happens, the PC of the victim instruction will be written into the EPC register. 
And when leaving from the ISR, software needs to read from EPC and then goes back to the interrupted instruction.
*/

HANDLE intMutex ;

/*The exception level bit is normally 0, 
but is set to 1 after an exception occurs.
When this bit is 1, interrupts are disabled and the EPC is not updated if another exception occurs. 
This bit prevents an exception handler from being disturbed by an interrupt or exception, 
but it should be reset when the handler finishes. 
If the interrupt enable bit is 1, interrupts are allowed. If it is 0, they are disabled.*/
/*The interrupt pending bits become 1 when an interrupt is raised at a given hardware or software level. 
The exception code register describes the cause of an exception through the following codes*/

extern void reg_init(void){
	intMutex= CreateMutex (NULL, FALSE, NULL);
	memset(reg, 0, REG_SIZE);
	memset(cpo_reg, 0, COPROCESSOR_REG_SIZE);
	cpo_reg[STATUS] = 0X0000FF13;// 1111111100010011
	//fix user mode to 1 (does not implement kernel mode)
	//enable all levels
}

//bit
bool bitState(reg_type reg, size_t index){
	reg_type mask = 1;
	mask <<= index;
	return ((reg & mask) != 0);
}

void SetBit(reg_type* reg_ptr, size_t index){
	reg_type mask = 1;
	mask <<= index;
	*reg_ptr |= mask;
}

void ClearBit(reg_type* reg_ptr, size_t index){
	reg_type mask = 1;
	mask <<= index;
	mask = ~mask;
	*reg_ptr &= mask;
}
//-----------for cause reg---------------------
bool interrupt(void){
	return (((cpo_reg[STATUS]&3)==3) && ((cpo_reg[STATUS]&cpo_reg[CAUSE]&0X0000FF00) != 0));
	//if enabled then check the mask & pending bit
}

void SetCause(size_t cause){//signal
	cpo_reg[CAUSE] &= 0XFFFFFF83;//clear
	//11..11 1000 0011
	cpo_reg[CAUSE] |= cause << 2;
	//write Cause
}

/*
//did by mips command
void ClearAllIntSignal(size_t index){//clear signal
	cpo_reg[Cause] = 0;
}*/