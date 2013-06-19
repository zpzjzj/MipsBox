/*
 * memory.c
 *
 *  Created on: Apr 16, 2013
 *      Author: zhaoping
 */
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <windows.h>
#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <list>

#include "memory.h"
#include "reg.h"
#include "header.h"

//#define LW_BUG
//#define SHFW_BUG
//#define MEM_ERROR

byte memory[MEMORY_SIZE];

pc_type gl_pc = 0;//program count
//pc is address

const size_t TOTAL_INT_NUM = 20;
//-------------------------------------begin of virtual address manipulate-------------------------

//const size_t TOTAL_VISITABLE_SIZE = 1024 * 1024; //1M => 1024K
std::list<size_t> page_list;
HANDLE PageMutex;
#define PAGE_DEFAULT 0XFFFFFFFF
//for LRU
//contains the index 1, 2, 3, 4
//ordered by the most recently used principle

static address_type page_table[MEMORY_SIZE/PAGE_SIZE] = 
	{PAGE_DEFAULT, PAGE_DEFAULT, PAGE_DEFAULT, PAGE_DEFAULT};
	//need to rectify afterwards perhaps
void memory_init(void){
	memset(memory, 0, sizeof(memory));
	memset(page_table, PAGE_DEFAULT, MEMORY_SIZE/PAGE_SIZE);
	for(size_t i = 0; i < MEMORY_SIZE/PAGE_SIZE; i++){
		page_list.push_back(i);
	}//initialize with 0, 1, 2, ... (page index)
	
	PageMutex= CreateMutex (NULL, FALSE, NULL);
	for(size_t i = 0; i < TOTAL_VISITABLE_SIZE / PAGE_SIZE; i++){
		char fileName[50];
		sprintf(fileName, "memfiles/Mem%d", i);
		FILE *fp = fopen(fileName, "wb");
		fwrite(memory, sizeof(byte), PAGE_SIZE, fp);
		fclose(fp);
	}//clear files
#ifdef MEM_ERROR
	puts("Inside memory_init():");
	std::list<size_t>::iterator end = page_list.end();
	size_t i = 0;
	for(std::list<size_t>::iterator it=page_list.begin(); it != end; it++){
		printf("page_list[%d]: %X\n", i, *it);
		i++;
	}
	puts("End of memory_init():");
#endif
}

address_type LRU(void){
#ifdef MEM_ERROR
	puts("Inside LRU():");
	printf("List size: %d\n", page_list.size());
	std::list<size_t>::iterator end = page_list.end();
	size_t i = 0;
	for(std::list<size_t>::iterator it=page_list.begin(); it != end; it++){
		printf("page_list[%d]: %X\n", i, *it);
		i++;
	}
	puts("End of LRU():");
#endif
	return page_list.back();//the last element
}

size_t MRU_index;
bool find_MRU(size_t index){
	return index == MRU_index;
}

void UpdatePageUseInfo(size_t dPage_index){
	//make sure that MRU is in the page_list
	//move dPage to the front
	MRU_index = dPage_index;
	if(page_list.front() == dPage_index){//do nothing
		return;
	}
	else{
		page_list.remove_if(find_MRU);
		page_list.push_front(dPage_index);
		return;
	}
}

address_type FindMemory(address_type addr){//find and change the pages
	void ReplacePage(address_type page[], 
				 size_t replace_index, address_type dPage);
//--------------------declaration of functions--------------------------
	page_type dPage = addr / PAGE_SIZE;
	address_type dOffset = addr % PAGE_SIZE;
	for(size_t i = 0; i < MEMORY_SIZE/PAGE_SIZE; i++){
		if(page_table[i] == dPage){//find
			UpdatePageUseInfo(i);//pull this page to the front of the list
#ifdef MEM_ERROR
	printf("memory: %X (actual: %X)\n", addr, PAGE_SIZE*i + dOffset);
	for(size_t j = 0; j < 8; j++){
		printf("%d ", memory[PAGE_SIZE*i + dOffset - 4 + j]);
	}
	putchar('\n');
#endif
			return PAGE_SIZE*i + dOffset;
		}
	}

#ifdef MEM_ERROR
	printf("addr: %u(0X%X)\n", addr, addr);
	printf("dPage: %u(0X%X)\n", dPage, dPage);
	printf("page_table: %u %u %u %u\n", page_table[0], page_table[1], page_table[2], page_table[3]);
#endif
	size_t replace_index = LRU();//find the least recently used page
#ifdef MEM_ERROR
	printf("LRU_index: %d\n", replace_index);
	printf("page_table[replace_index]: %d\n", page_table[replace_index]);
#endif
	ReplacePage(page_table, replace_index, dPage);
	UpdatePageUseInfo(replace_index);
#ifdef MEM_ERROR
	puts("After UpdatePageUseInfo()");
	printf("List size: %d\n", page_list.size());
	std::list<size_t>::iterator end = page_list.end();
	size_t i = 0;
	for(std::list<size_t>::iterator it=page_list.begin(); it != end; it++){
		printf("page_list[%d]: %X\n", i, *it);
		i++;
	}
#endif	
	ReleaseMutex(PageMutex);
#ifdef MEM_ERROR
	printf("memory: %X (actual: %X)\n", addr, PAGE_SIZE*replace_index + dOffset);
	for(size_t j = 0; j < 8; j++){
		printf("%d ", memory[PAGE_SIZE*replace_index + dOffset - 5 + j]);
	}
	putchar('\n');
#endif
	return PAGE_SIZE*replace_index + dOffset;
}

void ReplacePage(address_type page[], size_t replace_index,
				 address_type dPage){
	WaitForSingleObject(PageMutex, INFINITE);
	char fileName[50];
	FILE *fp;
	address_type mem_addr = PAGE_SIZE*replace_index;

	if(page[replace_index] != PAGE_DEFAULT){
		sprintf(fileName, "memfiles/Mem%d", page[replace_index]);
		fp = fopen(fileName, "wb");
		if(fp == NULL){
			printf("For file %s:\n", fileName);
			ERROR("Cannot open the mem file to write");
		}
#ifdef MEM_ERROR
	puts("before fwrite");
	printf("memory:  (actual: %X)\n", mem_addr);
	for(size_t j = 0; j < 8; j++){
		printf("%d ", memory[mem_addr - 4 + j]);
	}
	putchar('\n');
#endif
		size_t count;
		count = fwrite(&memory[mem_addr], sizeof(memory[mem_addr]), PAGE_SIZE, fp);//write
		fclose(fp);
	#ifdef MEM_ERROR
		puts("Inside ReplacePage()");
		printf("Page to write: %s\n", fileName);
	#endif
	}
	page[replace_index] = dPage;//replace
	sprintf(fileName, "memfiles/Mem%d", page[replace_index]);
	fp = fopen(fileName, "rb");
	if(fp == NULL){
		printf("For file %s:\n", fileName);
		ERROR("Cannot open the mem file to read");
	}
	fread(&memory[mem_addr], 1, sizeof(memory[mem_addr])*PAGE_SIZE, fp);//load
	fclose(fp);
#ifdef MEM_ERROR
	printf("Page to read: %s\n", fileName);
#endif
}

size_t memory_fread(address_type addr, size_t size, size_t count, FILE* fp){
	//load in file contents from memory[addr]
	// ...*****|***********|************
	address_type physical_addr = FindMemory(addr);
	size_t partial_read_size = PAGE_SIZE - physical_addr % PAGE_SIZE;
	size_t size_total = 0;

	while (size_total < size){
		size_t tmp_size;
		physical_addr = FindMemory(addr);//find current page
		tmp_size = fread(&memory[physical_addr], 1, partial_read_size, fp);
		size_total += tmp_size;
		if(tmp_size < partial_read_size)//read all
			break;
		addr += partial_read_size;
		partial_read_size = PAGE_SIZE;
	}

	return size_total;
}
//-------------------------------------end of virtual address manipulate-------------------------

//-------------------------------------begin of memory visit interface-------------------------------
//LITTLE_ENDIAN
const byte MASK = 0XFF;

void StoreWord(word data, address_type address){
	WaitForSingleObject(PageMutex, INFINITE);
	size_t i;
	for(i = 0; i < 4; i++, address++){
		StoreByte((byte)(data & MASK), address);
		data >>= 8; //8 bits
	}//store data with little endian principal
	ReleaseMutex(PageMutex);
}

word LoadWord(address_type address){
	WaitForSingleObject(PageMutex, INFINITE);
	word data = 0;
	size_t i;
	address += 3;
	for(i = 0; i < 4; i++, address--){
		data <<= 8;
		data |= LoadByte(address);
	}
	ReleaseMutex(PageMutex);
	return data;
}

void StoreHalfWord(half_word data, address_type address){
	WaitForSingleObject(PageMutex, INFINITE);
	size_t i;
	for(i = 0; i < 2; i++, address++){
		StoreByte((byte)(data & MASK), address);
		data >>= 8;//8 bits
	}//store data with little endian principal
	ReleaseMutex(PageMutex);
}

half_word LoadHalfWord(address_type address){
	WaitForSingleObject(PageMutex, INFINITE);
	half_word data = 0;
	address += 1;
	size_t i;
	for(i = 0; i < 2; i++, address--){
		data <<= 8;
		data |= LoadByte(address);
	}
	ReleaseMutex(PageMutex);
	return data;
}

void StoreByte(byte data, address_type address){
#ifdef MEM_ERROR
	puts("****************************************");
	puts("Inside StoreByte()");
	printf("data: %d address: %X\n", data, address);
#endif
	WaitForSingleObject(PageMutex, INFINITE);
	memory[FindMemory(address)] = data;
	ReleaseMutex(PageMutex);
}

byte LoadByte(address_type address){
#ifdef MEM_ERROR
	puts("****************************************");
	puts("Inside LoadByte()");
	printf("address: %X\n", address);
#endif
	WaitForSingleObject(PageMutex, INFINITE);
	ReleaseMutex(PageMutex);
	return memory[FindMemory(address)];
}


address_type GetTmpAddr(address_type addr){
#ifdef MEM_ERROR
	puts("-------------------------------------------------------");
	puts("Inside GetTmpAddr");
	printf("address: %X\n", addr);
	address_type tmp = FindMemory(addr);
	printf("physical address: %X\n", tmp);
	return tmp;
#endif
	return FindMemory(addr);
}
//-------------------------------------end of memory visit interface-------------------------------

void DisplayData(address_type addr){
	//display memory
	const int DISP_DATA_LINE_NUM = 8;
	address_type address = addr;
	size_t i, j, k;
	byte data;

	puts("Inside DisplayData");
	for(i = 0; i < DISP_DATA_LINE_NUM; i++){
		printf("%04X:\t", address);
		for(j = 0; j < 2; j++){
			for(k = 0; k < 8; k ++){//double word size
				data = LoadByte(address);
				printf("%02X ", data);
				address++;
				if(address >= TOTAL_VISITABLE_SIZE)
					goto END;
			}
			if(j == 0) putchar('-');
		}
		putchar('\n');
	}
	puts("End of DisplayData");
END:
	putchar('\n');
	return;
}
