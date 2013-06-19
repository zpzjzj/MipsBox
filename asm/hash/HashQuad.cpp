#include "HashQuad.h"
#include "GeneralHashFunctions.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define Error(X){puts(X);}
#define FatalError(X){puts(X); exit(0);}

//#define HASH_BUG
HashTable InitializeTable(int TableSize){
	unsigned int NextPrime(unsigned int inval);
	HashTable H;
	int i;

	const int MinTableSize = 5;
	if(TableSize < MinTableSize){
		Error("Table size too small");
		return NULL;
	}

	H = (struct HashTbl *)malloc(sizeof(struct HashTbl));
	if(H == NULL)
		FatalError("Out of space!!!");

	H->TableSize = NextPrime(TableSize);
	H->TheCells = (Cell *)malloc(sizeof(Cell) * (H->TableSize));
	if(H->TheCells == NULL)
		FatalError("Out of space!!!");

	for(i = 0; i < H->TableSize; i++)
		H->TheCells[i].Info = Empty;

	return H;
}

void DestroyTable(HashTable H){
	free(H->TheCells);
	free(H);
}
Position Find(ElementType Key, HashTable H){
	Position CurrentPos;
	int CollisionNum = 0;
#ifdef HASH_BUG
		printf("key: %s \n", Key);
#endif
	CurrentPos = ELFHash(Key, strlen(Key)) % H->TableSize;
#ifdef HASH_BUG
		printf("Hash(%s) -> %d\n", Key, CurrentPos);
#endif
	while(H->TheCells[CurrentPos].Info != Empty &&
			strcmp(H->TheCells[CurrentPos].Element, Key) != 0)
	{//collision 
#ifdef HASH_BUG
		printf("CurrentPos: %d, Element: %s\n", CurrentPos, H->TheCells[CurrentPos].Element);
#endif
		CurrentPos += 2 * (++CollisionNum) - 1;
		if(CurrentPos >= (Position)H->TableSize)
			CurrentPos -= (Position)H->TableSize;
	}
#ifdef HASH_BUG
		printf("CurrentPos: %d, Element: %s\n", CurrentPos, H->TheCells[CurrentPos].Element);
#endif
	return CurrentPos;
}

ElementReturnType Retrieve(Position P, HashTable H){
	//return a string according to place
	if(H->TheCells[P].Info != Legitimate){
		//deleted or empty that is not find
		return NULL;
	}
	else
		return H->TheCells[P].Element;
}

Position Insert(ElementType Key, HashTable H){
	Position Pos;

	Pos = Find(Key, H);
	if(H->TheCells[Pos].Info != Legitimate){
		H->TheCells[Pos].Info = Legitimate;
		strcpy(H->TheCells[Pos].Element, Key);
	}
	return Pos;
}

unsigned int NextPrime(unsigned int inval)
{
	#define PRIME_FALSE   0
	#define PRIME_TRUE    1
	unsigned int perhapsprime;      //Holds a tentative prime while we check it.
	unsigned int testfactor;
	//Holds various factors for which we test perhapsprime.
	int found;               /* Flag, false until we find a prime. */

	if (inval < 3 )          /* Initial sanity check of parameter. */
	{
	 if(inval <= 0) return(1);  /* Return 1 for zero or negative input. */
	 if(inval == 1) return(2);  /* Easy special case. */
	 if(inval == 2) return(3);  /* Easy special case. */
	}
	else
	{
	 /* Testing an even number for primeness is pointless, since
	  * all even numbers are divisible by 2. Therefore, we make sure
	  * that perhapsprime is larger than the parameter, and odd. */
	 perhapsprime = ( inval + 1 ) | 1 ;
	}
	/* While prime not found, loop. */
	for( found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2 )
	{
	 /* Check factors from 3 up to perhapsprime/2. */
	 for( testfactor = 3;
		  testfactor <= (perhapsprime >> 1);
		  testfactor += 1 )
	 {
	   found = PRIME_TRUE;      /* Assume we will find a prime. */
	   if( (perhapsprime % testfactor) == 0 )
	   {                   /* If testfactor divides perhapsprime... */
		 found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
		 goto check_next_prime; /* Break the inner loop,
								   go test a new perhapsprime. */
	   }
	 }
	 check_next_prime:; /* This label is used to break the inner loop. */
	 if( found == PRIME_TRUE )  /* If the loop ended normally,
								   we found a prime. */
	 {
	   return( perhapsprime );  /* Return the prime we found. */
	 }
	}
	return( perhapsprime );      /* When the loop ends,
								   perhapsprime is a real prime. */
}
