/*
 * GetStr.c
 *
 *  Created on: Apr 18, 2013
 *      Author: zhaoping
 */

#include "getStr.h"

char *GetStr(char *dStr, char *sStr){
	//extract a string to dStr from sStr
	char *s = sStr;
	char *d = dStr;
	while(*s == ' ' || *s == '\t' || *s == '\n') s++;//skip space
	while(*s != ' ' && *s != '\t' && *s != '\n' && *s != '\0'){
		*d = *s;
		d++;
		s++;
	}
	*d = '\0';
	while((*s == '\n') || (*s == ' ') || (*s == '\t')) s++;//skip space

	if(d == dStr) return sStr;//didn't get string
	else return s;
}
