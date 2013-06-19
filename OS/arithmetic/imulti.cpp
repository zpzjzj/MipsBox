/*
 * multi.c
 *
 *  Created on: Mar 22, 2013
 *      Author: zhaoping
 */
#include <stdlib.h>
#include "header.h"
#include "iadder.h"
#include <stdio.h>

LOperand_type __imul(operand_type multiplicand, operand_type multiplier){
	//2 bit booth
	//complement
	//signed
	LOperand_type product = 0;
	LOperand_type exMultiplier = (LOperand_type)multiplier;	//extended for (32bit + 1bit) to shift
	char mask = 0x7;//0111
	operand_type of, cf;

	LOperand_type multi = (LOperand_type)multiplicand;//0 + multiplicand
	multi <<= ((sizeof(operand_type) * 8));
	multi = ((int64_t)multi >> 2);//extended with sign bit
	LOperand_type multDouble = multi << 1;

	exMultiplier <<= 1;
	//add 0 to the tail of multiplier
	char lastThreeBit;
	size_t i;
	for(i = 0; i <= ((operand_size - 2) / 2); i++)	//2-bit booth algorithm
	{//calculate 16 times for 32 bits and shift 15 bits
		lastThreeBit = mask & exMultiplier;
		switch(lastThreeBit)
		{
			case 0://000 -> 0
			case 7://111 -> 0
				break;
			case 1://001 -> 1
			case 2://010 -> 1
				product = __iadd_64bit(product, multi, &cf, &of);
				break;
			case 3://011 -> 2
				product = __iadd_64bit(product, multDouble, &cf, &of);
				break;
			case 4://100 -> -2
				product = __isub_64bit(product, multDouble, &cf, &of);
				break;
			case 5://101 -> -1
			case 6://110 -> -1
				product = __isub_64bit(product, multi, &cf, &of);
				break;
		}
		if(i == (operand_size - 2) / 2)
			break;
		else{
			exMultiplier = (int64_t)exMultiplier >> 2;//shift with sign
			product = (int64_t) product >> 2;//shift with sign
		}
	}
	return product;
}

LOperand_type __idiv(LOperand_type dividend, operand_type divisor, operand_type *ptrRemainder){
	//signed
	unsigned char flag = 0;
	LOperand_type oldDividend = dividend;
	operand_type oldDivisor = divisor;
	if(dividend > ((LOperand_type)(0 - 1) >> 1)){
		//if negative
		flag = !flag;
		dividend = trans_compl_64bit(dividend);
	}
	if(divisor > ((operand_type)(0 - 1) >> 1)){
		//if negative
		flag = !flag;
		divisor = trans_compl(divisor);
	}

//	printf("idiv: dividend : %I64X, divisor: %X\n", dividend, divisor);
	LOperand_type quotient = 0;
	LOperand_type exDivisor = divisor;
	exDivisor <<= (sizeof(divisor) * 8);//divisor 0000000
//	printf("idiv: dividend : %I64X, divisor: %I64X\n", dividend, exDivisor);
//	printf("0X8000000000000000\n%I64X\n", (LOperand_type)(0 - 1));
	size_t count;
	for(count = 0; exDivisor < 0X4000000000000000; count++){
		//leave sign
		exDivisor <<= 1;
//		printf("count: %u, exDivisor: %I64X\n", count, exDivisor);
	}
//	printf("count: %u, exDivisor: %I64X\n", count, exDivisor);
	size_t i;

	operand_type of, cf;
	dividend = __isub_64bit(dividend, exDivisor, &cf, &of);
	for(i = 0; i <= (sizeof(divisor) * 8 + count); i++){
//		for sizeof(divisor)* 8 + 1 times -> for divisor to shift 8 times and sub then check
//		printf("i = %d, idiv: exDivisor: %I64X, quotient: %I64X, dividend: %I64X\n", i, exDivisor, quotient, dividend);
		exDivisor >>= 1;
		quotient <<= 1;
		if((int64_t)dividend < 0){
			dividend = __iadd_64bit(dividend, exDivisor, &cf, &of);
			//quotient[i] = 0;
		}
		else{
			dividend = __isub_64bit(dividend, exDivisor, &cf, &of);
			quotient |= 1;	//quotient[i] = 1;
		}
	}
	int32_t remainder;

	if(flag != 0){
		quotient = trans_compl_64bit(quotient);
	}

	remainder = oldDividend - __imul(quotient, oldDivisor);
//	printf("remainder: %d(%X)\n", remainder, remainder);
//	printf("remainder: %d(%X)\n", remainder,remainder);
/*	if(oldDividend > divisor)//absolute value
		remainder = oldDividend - quotient * divisor;
	else
		remainder = (int32_t)(oldDividend & 0XFFFFFFFF);//last 32 bits*/
	*ptrRemainder = remainder;

	return quotient;
}
