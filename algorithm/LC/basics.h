
#ifndef BASICSINCLUDED
#define BASICSINCLUDED


	/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

	/* data types */
#if defined(__BUILD_FOR_INTEL_DARWIN)
#else
  typedef unsigned long ulong;
#endif

typedef unsigned char byte;

typedef int mybool;
#define myTrue 1
#define myFalse 0

	/* distance --- depends on whether it is discrete or continuous */
#define CONT

#ifdef CONT
typedef float Tdist;
#else
typedef int Tdist;
#endif

#include "obj.h"

	/* random generators --- se usage examples in genqueries.c */

#define faleat (((unsigned)random())/((double)(unsigned)(1<<31)))
#define aleat(n) ((unsigned)((faleat)*(n)))

	/* global counter of distance computations */
extern long long numDistances;

extern long long pageAccess;

	/* global counter of deletions */
extern int numDeleted;

	/* finer memory management */

void *_free(void *p);
void *verif(void *p);
#define malloc(n) ((n)==0?NULL:verif((void*)malloc(n)))
#define realloc(p,n) ((p)==NULL?malloc(n):((n)==0?_free(p):verif((void*)realloc(p,n))))
#define free(p) { if ((p)!=NULL) free(p); }


	/* management of the list of candidates for NN queries */

typedef struct
   { Obj id;
     Tdist dist;
   } celem; // list object

typedef struct
   { celem *elems;
     int csize;
     int k;
   } Tcelem; // list

 // list of answers for range queries

typedef struct t_opair
{
  Tdist   d; // distance to the query
  Obj     o; // object
}opair; // object

typedef struct t_ret
{
  uint chunk;
  opair *ret;
  uint  iret;
}Tret; // answer
      
	/* creates a celem of size k */
Tcelem createCelem (int k);

	/* adds element if (distance dist) to celems */
void addCelem (Tcelem *celems, Obj id, Tdist dist);

	/* tells whether dist is out of the range of celem */
mybool outCelem (Tcelem *celems, Tdist dist);

	/* prints the objects of celem */
void showCelem (Tcelem *celems);

	/* gives the radius of the farthest element in celem, -1 if empty */
Tdist radCelem (Tcelem *celems);

	/* frees celem */
void freeCelem (Tcelem *celems);


	/* inserts Or in ret if it is at distance at most r from query o */
mybool insRet(Tret *ret,Obj Or, Obj o, Tdist r);

	/* inserts Or in ret */
void FinsRet(Tret *ret,Obj Or); 

	/* inits ret with realloc step 3 */
void iniciaRet(Tret *ret);

#endif
