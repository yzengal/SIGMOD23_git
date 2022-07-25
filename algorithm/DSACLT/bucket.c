#include <stdio.h>
#include "obj.h"
#include "bucket.h"
#include "basics.h"

#define BLK 5

bucket createbucket (void)

   { return NULL;
   }

bucket addbucket (bucket B, int bsize, Obj obj)

   { if ((bsize % BLK) == 0)
        B = (bucket) realloc (B,(bsize+BLK)*sizeof(int));
     B[bsize] = obj;
     return B;
   }

Obj elembuck (bucket B, int pos)

   { return B[pos];
   }

void freebucket (bucket B, int bsize)

   { free (B);
   }
	     
int searchbucket (bucket B, int bsize, Obj obj, Tdist r, mybool show)

   { int i;
     int rep = 0;
     for (i=0;i<bsize;i++)
	 if (distance(B[i],obj) <= r) 
	    { rep++; if (show) printobj(B[i]); }
     return rep;
   }

void searchbucketNN (bucket B, int bsize, Obj obj, Tcelem *res)

   { int i;
     Tdist dist;
     for (i=0;i<bsize;i++)
	 { dist = distance(B[i],obj);
	   addCelem(res,B[i],dist);
	 }
   }

void savebucket (bucket B, int bsize, FILE *f)

   { fwrite(B, bsize*sizeof(int),1,f);
   }


bucket loadbucket (int bsize, FILE *f)

   { bucket B = (bucket) malloc (((bsize+BLK-1)/BLK)*BLK*sizeof(void*));
     fread(B,bsize*sizeof(int),1,f);
     return B;
   }
   
Tdist distanceInter (Obj obj1, Obj obj2)
{
	assert(obj1>=0 && obj1<nV);
	assert(obj2>=0 && obj2<nV);
	return  dist(V[obj1], V[obj2], tag);
}

void printobj (Obj obj)

   { printf ("%d\n", obj);
   }