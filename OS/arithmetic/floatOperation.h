/*
 * floatOperation.h
 *
 *  Created on: Apr 7, 2013
 *      Author: zhaoping
 */

#ifndef FLOATOPERATION_H_
#define FLOATOPERATION_H_

extern const float_type INF_EXP;
extern const float_type NaN;//quiet NaN
extern const float_type POS_INF_FL;//positive infinity
extern const float_type NEG_INF_FL;//negative infinity
extern const operand_type HIGHEST_BIT;//32-bit 1000 0000 0000 ...
extern const LOperand_type L_HIGHEST_BIT;//64-bit 1000 0000 0000 ...
extern const operand_type EXPO_PART;
//exponent part(8-bit) 0111 1111 1000
extern const operand_type SIGNI_PART;
//significant part (8-bit) 0111 1111 1111
extern const int EXPO_POS;//exponent's position

extern operand_type Trans_To_Fl(LOperand_type num, int *pExponent, int sign);
//transform number, pExponent, sign to float
#define INF(x) (((x) == POS_INF_FL) || ((x) == NEG_INF_FL))
//infinity check (S=1,0  E=FF, M==0)
#define NAN(x) ((((x) & EXPO_PART) == EXPO_PART) && (((x) & SIGNI_PART) != 0))
//not a number check (S=1,0  E=FF M!=0)
#define ZERO(x) ((x & 0x7FFFFFFF) == 0)
//S=1,0  E=0, M=0

#define GetExponent(x) (((x) & EXPO_PART) >> EXPO_POS)

//# define ReachLen24(x) (x >= 0X800000)	//1 for reach
# define OverLen24(x) (x >= 0X1000000)	//1 for over
# define LAST_24_BITS 0xFFFFFF

extern float_type __fadd(float_type, float_type);
extern float_type __fsub(float_type op1, float_type op2);//return float op1 - op2
extern float_type __fmul(float_type, float_type);
extern float_type __fdiv(float_type, float_type);
extern operand_type Trans_To_Fl(LOperand_type num, int *pExponent, int sign);

#endif /* FLOATOPERATION_H_ */
