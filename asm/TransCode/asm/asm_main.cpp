#include "Assemble.h"

int main(){
	puts("Input the PC to start (hex)");
	pc_type pc;
	scanf("%X", &pc);
	getchar();//eat endl

	Assemble(pc);
	char fileName[30];

	//input the whole name including .com or sth
	PrintCommandCode();
	puts("Input the file name to write:");
	gets(fileName);
	WriteCodeToFile(fileName);
	return 0;
}
