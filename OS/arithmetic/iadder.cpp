/*
 * iadder.c
 *
 *  Created on: Mar 22, 2013
 *      Author: zhaoping
 */
# include "header.h"
# include <stdlib.h>
# include <ctype.h>

#include "iadder.h"


//#define ATOI_ERROR
#ifdef ATOI_ERROR
#include <stdio.h>
#endif
//#define IADD_ERROR
#ifdef IADD_ERROR
#include <stdio.h>
#endif
//#define IADD_64_ERROR
#ifdef IADD_64_ERROR
#include <stdio.h>
#endif

operand_type trans_compl(operand_type num){
	num ^= (operand_type)(0 - 1);
	num += 1;
	return num;
}

LOperand_type trans_compl_64bit(LOperand_type num){
	//help trans negative number into it's complement
	num ^= (LOperand_type)(0 - 1);	// ~ each bit
	num += 1;
	return num;
}

LOperand_type __atoi(char *s)		//input number in decimal
{
	char flag = 0;
	LOperand_type num = 0;

	while(isspace(*s))	s++;

	do{
		if(*s == '-'){
#ifdef ATOI_ERROR
		putchar(*s);
#endif
			flag = 1;	//detect negtive number
		}
		else if(isdigit(*s))	//trans char [] to num
		{
			do{
#ifdef ATOI_ERROR
		printf("Inside isdigit while\n");
		putchar(*s);
#endif
				num *= 10;
				num += *s - '0';
				s++;
			}
			while(isdigit(*s));
			if(*s == '\0')
				break;
		}
		s++;
	}while(*s != '\0');
	if(flag == 1)	//if negative then take its complement
	{
		num = trans_compl_64bit(num);
	}
#ifdef ATOI_ERROR
		printf("num: %I64d\n", num);
#endif
	return num;		//return is here
}

char * __itoa(operand_type num, char *str_num){
	char temp[MAX_STR_LEN];
	int i = 0;
	char *s = str_num;
	if(num > ((operand_type)(0 - 1) >> 1)){
		*s = '-';
		s++;

		num = trans_compl(num);
	}//if short unsigned int detect it's sign

	i = 0;
	do{
		temp[i] = num % 10 + '0';
		num /= 10;
		i++;
	}while(i < MAX_STR_LEN && num > 0);
	//suppose i < STR_LEN is guarded

	i--;
	for( ; i >= 0; i--, s++){
		*s = temp[i];
	}
	*s = '\0';

	return str_num;
}
char * __itoa_64bit(LOperand_type num, char *str_num)
{
	char temp[MAX_STR_LEN];
	int i = 0;
	char *s = str_num;

	if(num > ((LOperand_type)(0 - 1) >> 1)){
		//detect the first sign bit
		*s = '-';
		s++;

		num = trans_compl_64bit(num);
	}
	i = 0;
	do{
		temp[i] = num % 10 + '0';
		num /= 10;
		i++;
	}while(i < MAX_STR_LEN && num > 0);
	//suppose i < STR_LEN is guarded

	i--;
	for( ; i >= 0; i--, s++){
		*s = temp[i];
	}
	*s = '\0';

	return str_num;
}
operand_type __adder_1bit(operand_type op1, operand_type op2, operand_type carry1, operand_type *carry2);
LOperand_type __iadd_64bit(LOperand_type op1, LOperand_type op2, operand_type *ptrCF, operand_type *ptrOF){
	const size_t LOperand_size = 64;
	operand_type arr_1bits_op1[LOperand_size];
	operand_type arr_1bits_op2[LOperand_size];
	LOperand_type res = 0;
	int i;
	LOperand_type mask = 1;
	for(i = 0; i < LOperand_size; i++){
		arr_1bits_op1[i] = ((mask & op1) != 0);
		arr_1bits_op2[i] = ((mask & op2) != 0);
		mask <<= 1;
	}
#ifdef IADD_64_ERROR
	printf("op1: %I64X\n", op1);
	for(i = LOperand_size; i >= 0; i--){
		printf("%d ", arr_1bits_op1[i]);
	}
	putchar('\n');

	printf("op2: %I64X\n", op2);
	for(i = LOperand_size; i >= 0; i--){
		printf("%d ", arr_1bits_op2[i]);
	}
	putchar('\n');
#endif
	operand_type arr_1bits_carry[LOperand_size];
	operand_type arr_1bits_res[LOperand_size];
	arr_1bits_carry[0] = 0;
	arr_1bits_res[0] = __adder_1bit(arr_1bits_op1[0], arr_1bits_op2[0], arr_1bits_carry[0], &arr_1bits_carry[1]);
	for(i = 1; i < LOperand_size; i++){
		arr_1bits_res[i] = __adder_1bit(arr_1bits_op1[i], arr_1bits_op2[i], arr_1bits_carry[i-1], &arr_1bits_carry[i]);
	}

	mask = 0X8000000000000000;
	for(i = LOperand_size - 1; i >= 0; i--){
		if(arr_1bits_res[i] == 1){
			res |= mask;
		}
		mask >>= 1;
	}
#ifdef IADD_64_ERROR
	printf("res: %I64X\n", res);
	for(i = LOperand_size; i >= 0; i--){
		printf("%d ", arr_1bits_res[i]);
	}
	putchar('\n');
#endif

	if(arr_1bits_carry[LOperand_size-1] == 1) *ptrCF = 1;
	else *ptrCF = 0;
	if((arr_1bits_carry[LOperand_size-1] ^ arr_1bits_carry[LOperand_size-2]) == 1) *ptrOF = 1;
	else *ptrOF = 1;
	return res;
}
operand_type __adder_1bit(operand_type op1, operand_type op2, operand_type carry1, operand_type *carry2){
	operand_type res;
	res = op1 ^ op2 ^ carry1;
	*carry2 = (op1 & op2) || (op1 & carry1) || (op2 & carry1);
	return res;
}


operand_type __iadd(operand_type op1, operand_type op2, operand_type *ptrCF, operand_type *ptrOF){
	//return (op1 + op2);
	operand_type arr_1bits_op1[operand_size];
	operand_type arr_1bits_op2[operand_size];
	operand_type res = 0;
	int i;
	operand_type mask = 1;
	for(i = 0; i < operand_size; i++){
		arr_1bits_op1[i] = ((mask & op1) != 0);
		arr_1bits_op2[i] = ((mask & op2) != 0);
		mask <<= 1;
	}
#ifdef IADD_ERROR
	printf("op1: %X\n", op1);
	for(i = operand_size; i >= 0; i--){
		printf("%d ", arr_1bits_op1[i]);
	}
	putchar('\n');

	printf("op2: %X\n", op2);
	for(i = operand_size; i >= 0; i--){
		printf("%d ", arr_1bits_op2[i]);
	}
	putchar('\n');
#endif
	operand_type arr_1bits_carry[operand_size + 1];
	operand_type arr_1bits_res[operand_size];
	arr_1bits_carry[0] = 0;
	arr_1bits_res[0] = __adder_1bit(arr_1bits_op1[0], arr_1bits_op2[0], arr_1bits_carry[0], &arr_1bits_carry[1]);
	for(i = 1; i < operand_size; i++){
		arr_1bits_res[i] = __adder_1bit(arr_1bits_op1[i], arr_1bits_op2[i], arr_1bits_carry[i], &arr_1bits_carry[i+1]);
	}
	mask = 0X80000000;
	for(i = operand_size - 1; i >= 0; i--){
		if(arr_1bits_res[i] == 1){
			res |= mask;
		}
		mask >>= 1;
	}
#ifdef IADD_ERROR
	printf("res: %X\n", res);
	for(i = operand_size; i >= 0; i--){
		printf("%d ", arr_1bits_res[i]);
	}
	putchar('\n');
#endif

	if(arr_1bits_carry[operand_size-1] == 1) *ptrCF = 1;
	else *ptrCF = 0;
	if((arr_1bits_carry[operand_size-1] ^ arr_1bits_carry[operand_size-2]) == 1) *ptrOF = 1;
	else *ptrOF = 0;

	return res;
}

operand_type __isub(operand_type op1, operand_type op2, operand_type *ptrCF, operand_type *ptrOF) //op1 - op2
{
	op2 = trans_compl(op2);//op1 - op2 => op1 + (-op2)
	return __iadd(op1, op2, ptrCF, ptrOF);
}

LOperand_type __isub_64bit(LOperand_type op1, LOperand_type op2, operand_type *ptrCF, operand_type *ptrOF) //op1 - op2
{
	op2 = trans_compl_64bit(op2);//op1 - op2 => op1 + (-op2)
	return __iadd_64bit(op1, op2, ptrCF, ptrOF);
}
