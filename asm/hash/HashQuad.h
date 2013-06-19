#ifndef _HASHQUAD_H
#define _HASHQUAD_H

#include "..\lenDef.h"
typedef char ElementType[MAX_INSTR_FIELD_STR_LEN];
typedef char *ElementReturnType;

typedef unsigned int Index;
typedef Index Position;


enum KindOfEntry {Legitimate, Empty, Deleted};
struct HashEntry{
	ElementType Element;
	enum KindOfEntry Info;
};
typedef struct HashEntry Cell;
struct HashTbl{
	int TableSize;
	Cell *TheCells;
};

typedef struct HashTbl *HashTable;

HashTable InitializeTable(int TableSize);
void DestroyTable( HashTable H);
Position Find(ElementType key, HashTable H);
Position Insert(ElementType Key, HashTable H);
ElementReturnType Retrieve( Position P, HashTable H);
#endif
