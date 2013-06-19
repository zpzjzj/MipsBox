#include <stdio.h>

int main(void){
	size_t i = 0;
	const size_t TOTAL_MEM_SIZE = 1024 * 1024; //1M
	const size_t PAGE_SIZE = 1024;	//1K

	FILE *fp;
	char fileName[30];
	for(i = 0; i < TOTAL_MEM_SIZE/PAGE_SIZE; i++){
		sprintf(fileName, "Mem%d", i);
		fp = fopen(fileName, "wb");
		fclose(fp);
	}
	return 0;
}
