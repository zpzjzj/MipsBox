/*
 * fArith.c
 *
 *  Created on: Mar 28, 2013
 *      Author: zhaoping
 */
# include "header.h"
# include <stdio.h>
# include <ctype.h>
# include <string.h>
#include "iadder.h"
#include "imulti.h"

#define F_DIV_ERROR

const float_type INF_EXP = 0xFF;//infinity's exponent
const float_type NaN = 0x7FC00000;//quiet NaN
const float_type POS_INF_FL = 0x7F800000;//positive infinity
const float_type NEG_INF_FL = 0xFF800000;//negative infinity
const operand_type HIGHEST_BIT = 0x80000000;//32-bit 1000 0000 0000 ...
const LOperand_type L_HIGHEST_BIT = 0x8000000000000000;//64-bit 1000 0000 0000 ...
const operand_type EXPO_PART = 0x7F800000;
//exponent part(8-bit) 0111 1111 1000
const operand_type SIGNI_PART = 0x07FFFFF;
//significant part (8-bit) 0111 1111 1111
const int EXPO_POS = 23;//exponent's position

//all of exponent variable here should be move number but signed for check

float_type __fdiv(float_type dividend, float_type divisor){
//	printf("dividend: %X divisor: %X\n", dividend, divisor);
	float_type quotient = 0;
	int flag = ((dividend & HIGHEST_BIT) != 0) ^ ((divisor & HIGHEST_BIT) != 0);
	
	if(NAN(dividend) || NAN(divisor)) return NaN;//deal with NaN
	if(ZERO(dividend)){
		if(ZERO(divisor)) return NaN;// 0/0
		else return 0;// 0/x
	}
	else if(ZERO(divisor)) return (flag == 0 ? POS_INF_FL: NEG_INF_FL);// a/0
	else if(INF(dividend) && INF(divisor)) return NaN;// INF/INF
	else if(INF(dividend)) return dividend;

	int expoDividend, expoDivisor;
	expoDividend =	GetExponent(dividend);
	expoDivisor = GetExponent(divisor);

	dividend &= SIGNI_PART;
	divisor &= SIGNI_PART;
	
	dividend ^= (0x1 << EXPO_POS);//add 1 at the first bit
	divisor ^= (0x1 << EXPO_POS);

//	printf("after leave signification only:  dividend: %X divisor: %X\n", dividend, divisor);
	LOperand_type lDividend = dividend;//64-bit
	lDividend <<= 2 * operand_size - (EXPO_POS + 1) - 1;//64 - 24 - 1 = 39 (sign = 0)

	LOperand_type divRes;
	operand_type remainder;
	divRes = __idiv(lDividend, divisor, &remainder);
//	printf("%I64X / %X = %I64X(accurate: %I64X)\n", lDividend, divisor, divRes, (LOperand_type)(lDividend / divisor));
	int expoQuotient = expoDividend - expoDivisor + 127 + EXPO_POS + 1;
	quotient = Trans_To_Fl(divRes, &expoQuotient, flag);

	return quotient;
}

float_type __fmul(float_type multiplicand, float_type multiplier){
	float_type product = 0;
	int flag;
	flag = ((multiplier & HIGHEST_BIT) != 0) ^ ((multiplicand & HIGHEST_BIT) != 0);//get flag
	
	if(NAN(multiplicand) || NAN(multiplier)) return NaN;//deal with NaN
	if((ZERO(multiplicand) && INF(multiplier)) || (ZERO(multiplier) && INF(multiplicand))){
		return NaN;
	}//0 * INF
	else if(ZERO(multiplicand) || ZERO(multiplier)) return 0;
	else if(INF(multiplicand) || INF(multiplier)) return (flag == 0 ? POS_INF_FL: NEG_INF_FL);// a/0
/*	printf("multiplicand: %p\n", (void *)multiplicand);
	printf("multiplier: %p\n", (void *)multiplier);*/

	int exponentMultiplicant, exponentMultiplier, exponentRes;
	exponentMultiplicant = GetExponent(multiplicand);
	exponentMultiplier = GetExponent(multiplier);
	//get exponent
/*	printf("exponentMultiplicant: %d\n", exponentMultiplicant);
	printf("exponentMultiplicant: %d\n", exponentMultiplier);*/

//	printf("flag: %d\n", flag);
	multiplicand &= SIGNI_PART;
	multiplier &= SIGNI_PART;

	if(multiplicand == 0 && exponentMultiplicant == 0)
		return 0;
	if(multiplier == 0 && exponentMultiplier == 0)
		return 0;
	if(exponentMultiplicant == INF_EXP || exponentMultiplier == INF_EXP){
		return flag ? NEG_INF_FL:POS_INF_FL;
	}

	multiplicand ^= (0x1 << EXPO_POS);//add 1 at the first bit
	multiplier ^= (0x1 << EXPO_POS);
	//1XXXX format
/*
	printf("multiplicand: %p\n", (void *)multiplicand);
	printf("multiplier: %p\n", (void *)multiplier);
*/
	LOperand_type mulRes = __imul(multiplicand, multiplier);
//	printf("%X * %X = %I64X(accurate: %I64x)\n", multiplicand, multiplier, mulRes, (LOperand_type)multiplier*multiplicand);
	exponentRes = exponentMultiplier + exponentMultiplicant - 127 + 2 * operand_size - (2 * EXPO_POS + 1);
/*
	printf("exponentRes: %d\n", exponentRes);
	printf("flag: %d\n", flag);*/
	product = Trans_To_Fl(mulRes, &exponentRes, flag);
//	printf("product: %p\n", (void *)product);
	return product;
}

operand_type Trans_To_Fl(LOperand_type num, int *pExponent, int sign){
	//format: 64-bit 0..01X..X highest bit with *pExponent
	if(num == 0){	//no 1 in the string
		(*pExponent) = 0;
		return num;
	}
	//sign = 1 => negative
	//1XXXX... or 0...1XXXXX * 2^exp => 1.XXXX * 2^(exp_new)
	LOperand_type mask = (LOperand_type)HIGHEST_BIT << operand_size;
	int i;
	for(i = 0; i < (2 * operand_size - (EXPO_POS + 1)); i++){
		//leave the last 23 bits any way
		if((mask & num) != 0){//find the first one
			break;
		}
		else{
			mask >>= 1;
			(*pExponent)--;
		}
	}//get *pExponent and find the first 1
	if(*pExponent >= 0XFF){
		return sign ? NEG_INF_FL:POS_INF_FL;
	}
	else if(*pExponent < 0){//too small
		return 0;
	}
//	printf("i = %d\n", i);

	operand_type res = 0;
	for(i = 1; i <= 24; i++){
		//get the first 24 bits (including the first 1)
//		printf("i = %d, num: %I64X\n", i, num);
		if((mask & num) != 0){
			res ^= 1;//add 1
			num ^= mask;//clear
		}
		if(i == 24)
			break;
		else{
			res <<= 1;
			mask >>= 1;
		}
	}//with 1.M on the rightmost
/*
	printf("res = %x\n", res);
	printf("i = %d, exponent = %d\n", i, *pExponent);*/
	res ^= (operand_type)0x1 << EXPO_POS; //eliminate 1
//	printf("after eliminate the first 1: %x\n", res);
	res |= ((*pExponent) << EXPO_POS);//add exponent
//	printf("after add exponent num: %x\n", res);
	res |= HIGHEST_BIT * sign;//add sign
/*	printf("after add sign num: %p\n", (void*)res);
	printf("final num: %p\n", (void*)res);*/
	return res;
}

float_type __fadd(float_type op1, float_type op2){
	int exponent_op1, exponent_op2;
	exponent_op1 = GetExponent(op1);
	exponent_op2 = GetExponent(op2);

	if(exponent_op1 < exponent_op2){
		float_type temp = op1;
		op1 = op2;
		op2 = temp;

		exponent_op1 = GetExponent(op1);
		exponent_op2 = GetExponent(op2);
	}//swap to make sure exponent_op1 >= exponent_op2

	int flag_op1 = 0, flag_op2 = 0;
	if((op1 & HIGHEST_BIT) != 0){//negative
		flag_op1 = 1;
	}
	if((op2 & HIGHEST_BIT) != 0){//negative
		flag_op2 = 1;
	}
//	printf("flag_op1: %d, flag_op2 : %d\n", flag_op1, flag_op2 );
//	printf("op1: %X, op2: %X\n", op1, op2);
	if(INF(op1) && INF(op2)){
		if((flag_op1 ^ flag_op2) == 1) return NaN;//+INF - INF
		else return (flag_op1 == 0 ? POS_INF_FL:NEG_INF_FL);
	}
	if(INF(op1) && INF(op2) && ((flag_op1 ^ flag_op2) == 1)) return NaN;//+INF - INF
	else if(ZERO(op1)) return op2;
	else if(ZERO(op2)) return op1;

//	printf("after swap: op1: %X, op2: %X\n", op1, op2);
//	printf("exponent_op1: %d, exponent_op2 : %d\n", exponent_op1, exponent_op2);
	op1 &= SIGNI_PART;
	op2 &= SIGNI_PART;//get significant part

	op1 |= (0x1 << EXPO_POS);//add 1 at the first bit
	op2 |= (0x1 << EXPO_POS);
//	printf("after keep only the significant:  op1: %X, op2: %X\n", op1, op2);

	unsigned int expoDiffr = exponent_op1 - exponent_op2;//difference of exponents
	//exponent_op1 > exponent_op2 => 0< expoDiffr < exponent_op1
	LOperand_type operator1 = op1, operator2 = op2;
	const int shiftSpan = EXPO_POS;//largest left shift span  23 for float

	int exponentRes;//must be signed
	unsigned int cf, of;
	if(expoDiffr < shiftSpan){
		//difference < 23 
		operator1 <<= expoDiffr;//shift left for addition later
		exponentRes = __iadd(exponent_op1, operand_size * 2 - EXPO_POS - expoDiffr - 1, &cf, &of);
	}
	else{//defference >= 23 
		operator2 = 0;
		exponentRes = exponent_op1;
		exponentRes = exponent_op1 + (operand_size * 2 - EXPO_POS - 1);
	}
//	printf("after shift:  operator1: %I64X, operator2: %I64X, exponentRes: %d\n", operator1, operator2, exponentRes);

	LOperand_type addRes;
	int sameSignFlag = !(flag_op2 ^ flag_op1);//if same then flag = 1;
	if(sameSignFlag == 1)
		addRes = __iadd_64bit(operator1, operator2, &cf, &of);
	else
		addRes = __isub_64bit(operator1, operator2, &cf, &of);

//	printf("addRes: %I64X\n", addRes);

	int flag = flag_op1;
	//to make addRes extended to 64 bit and the highest bit with exponentRes
	if((addRes & L_HIGHEST_BIT) != 0 && (sameSignFlag != 1)){
		//operator1 - operator2 < 0 same exponent and |operator1| < |operator2|
		flag = !flag;
		addRes = trans_compl_64bit(addRes);
	}

//	printf("addRes: %I64X\n", addRes);
	operand_type flAddRes; 
	flAddRes = Trans_To_Fl(addRes, &exponentRes, flag);

	return flAddRes;
}

float_type __fsub(float_type op1, float_type op2){
	float_type subRes;
	op2 ^= HIGHEST_BIT;
	subRes = __fadd(op1, op2);
	return subRes;
}
