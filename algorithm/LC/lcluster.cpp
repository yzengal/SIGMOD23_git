/**
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "lcluster.h"

void prnstats (Index S);
static int pageIdx;

	// strategy to select next center. left as random, others can be
	// activated by defining P2..P5 instead

int compar (const void *a, const void *b)

   {
     Tod *x = (Tod *)a;
     Tod *y = (Tod *)b;

     if (x->dist > y->dist) return 1;
     else if (x->dist < y->dist) return -1;
     else return x->obj < y->obj;
   }

static vpnode buildvpt (vpt *tree, Tod *od, int nobjs)

   { int i,per;
     vpnode node;
     
      int j = aleat (nobjs);
      node.u.interno.query = od[j].obj;
	  for (i=0;i<nobjs;i++) 
	  { od[i].dist = distance (node.u.interno.query, od[i].obj);
		od[i].tdist += od[i].dist;
	  }
	  // swap(od[0], od[j]);
	  swap(od[0], od[j]);

      qsort (od+1,nobjs-1,sizeof(Tod),compar);
	  assert(od[0].obj == node.u.interno.query);
	  per = (nobjs>tree->bsize) ? tree->bsize : nobjs;
	  node.u.interno.dist = od[per-1].dist;
	  node.u.hoja.bucket = createbucket();
	  node.u.hoja.size = 0;
	  for (i=0; i<per; i++) 
	  { node.u.hoja.bucket = (void *) addbucket ((bucket)node.u.hoja.bucket, node.u.hoja.size, od[i].obj);
	    node.u.hoja.size++;
	  }
	  
	  // dump to File
	  node.pageId = pageIdx;
	  int* pids = (int *)node.u.hoja.bucket;
	  for (i=0; i<node.u.hoja.size; i+=pageSize) {
		int n_out = min(pageSize, node.u.hoja.size-i);
		dumpToDisk(pageIdx++, pids+i, n_out);
	  }
	  
	  if (nobjs-per <= 0) {
		  node.u.interno.child2 = NULL;
	  } else {
		  node.u.interno.child2 = (svpnode *) malloc (sizeof(vpnode));
		  *node.u.interno.child2 = buildvpt (tree,od+per,nobjs-per);
	  }
	  
     return node;
   }

   
Index build (char *dbname, int n, int maxLeafCapacity)
{ 
	vpt *tree;
	int i, j;
	Tod *od;
	int nobjs = n;
	pageIdx = 0;
	
	tree = (vpt *) malloc (sizeof(vpt));
	tree->descr = (char *) malloc (strlen(dbname)+1);
	strcpy (tree->descr, dbname);
	tree->bsize = maxLeafCapacity;
	tree->np = n;
	
	if (n && (n < nobjs)) nobjs = n; 

	od = (Tod *) malloc (nobjs * sizeof (Tod));

	j=0; i=0;

	while (j < nobjs) { od[i].tdist = 0;  od[i++].obj = j++;  }
	tree->node = buildvpt (tree,od,nobjs);
	free (od);
	prnstats((Index)tree); 
	return (Index)tree;
}


static void freevpt (vpnode *node)

   { freebucket ((bucket)node->u.hoja.bucket,node->u.hoja.size);
     if (node->u.interno.child2 != NULL) 
	{ 
	  freevpt (node->u.interno.child2);
	  free (node->u.interno.child2);
	}
   }

void freeIndex (Index S, mybool libobj)

   { vpt *tree = (vpt*)S;
     free (tree->descr);
     freevpt (&tree->node);
     free (tree);
	 removeFromFile(pageIdx);
   }

static void savenode (vpnode *node, int bsize, FILE *f)

   { char hoja = node->pageId ? '1' : '0';
     putc (hoja,f);
     if (node->pageId)
	{ fwrite (&node->u.hoja.size,sizeof(int),1,f);
          savebucket ((bucket)node->u.hoja.bucket,node->u.hoja.size,f);
	}
     else
	{ fwrite(&node->u.interno.query,sizeof(int), 1, f);
	  fwrite (&node->u.interno.dist, sizeof(Tdist),1,f);
	  savenode(node->u.interno.child2,bsize,f);
	}
   }

void saveIndex (Index S, char *fname)

   { FILE *f = fopen(fname,"w");
     vpt *tree = (vpt*)S;
     fwrite (tree->descr,strlen(tree->descr)+1,1,f);
     fwrite (&tree->bsize,sizeof(int),1,f);
     savenode (&tree->node,tree->bsize,f);
     fclose (f);
   }

static void loadnode (vpnode *node, int bsize, FILE *f)

   { char hoja;
     hoja = getc(f);
     node->pageId = (hoja == '1');
     if (node->pageId)
	{ fread (&node->u.hoja.size,sizeof(int),1,f);
	  node->u.hoja.bucket = loadbucket (node->u.hoja.size,f);
	}
     else
	{ fread(&node->u.interno.query, sizeof(int), 1, f);
	  fread (&node->u.interno.dist,sizeof(Tdist),1,f);
	  node->u.interno.child2 = (svpnode *) malloc (sizeof(vpnode));
	  loadnode (node->u.interno.child2,bsize,f);
	}
   }

Index loadIndex (char *fname)

   { char str[1024]; char *ptr = str;
     FILE *f = fopen(fname,"r");
     vpt *tree = (vpt *) malloc (sizeof(vpt));

     while ((*ptr++ = getc(f)));
     tree->descr = (char *) malloc (ptr-str);
     strcpy (tree->descr,str);
     // tree->np=openDB (str);
     fread (&tree->bsize,sizeof(int),1,f);
     loadnode (&tree->node,tree->bsize,f);
     fclose (f);
     return (Index)tree;
   }
   
static int __searchBucket (location_t *locs, vpnode *node, Obj obj, Tdist r, mybool show) {
	int pageId = node->pageId, bucketSize = node->u.hoja.size;
	int *bucket = (int *)node->u.hoja.bucket;
	int rep = 0;
	
	for (int i=0; i<bucketSize; i+=pageSize) {
		int n_out = min(pageSize, bucketSize-i);
		if (i > 0) {
			readFromDisk(pageId, locs, n_out);
			pageAccess++;
		}
		for (int j=0; j<n_out; ++j) {
			if (dist(V[obj], locs[j], tag) <= r) {
			// if (dist(V[obj], V[bucket[i+j]], tag) <= r) {
				rep++; if (show) printobj(bucket[i+j]);
			}
		}
		pageId++;
	}
	
	return rep;
}

static int _search (vpnode *node, Obj obj, Tdist r, mybool show)

   { 
     if (node == NULL) return 0;
     int rep = 0;
	 
	 // read the center object
	 int n_out = min(pageSize, node->u.hoja.size);
	 location_t *locs =  (location_t *) malloc (sizeof(location_t) * n_out);
	 readFromDisk(node->pageId, locs, n_out);
	 pageAccess++;
	 Tdist distToCenter = dist(V[obj], locs[0], tag);
	 // Tdist distToCenter = dist(V[obj], V[node->u.interno.query], tag);
     if (distToCenter-r <= node->u.interno.dist)
	{ 
		pageAccess--;
		rep += __searchBucket (locs, node, obj, r, show);
	}
     if (node->u.interno.child2 != NULL)
	{  
	  if (distToCenter+r >= node->u.interno.dist)
	     rep += _search(node->u.interno.child2,obj,r,show);
	}
	
	 free(locs);
     return rep;
   }

int search (Index S, Obj obj, Tdist r, int show)

   { vpt *tree = (vpt*)S;
	 pageAccess = 0;
     return _search (&tree->node,obj,r,show);
   }
   
static void __searchBucketNN (location_t *locs, vpnode *node, Obj obj, Tcelem *res) {
	int pageId = node->pageId, bucketSize = node->u.hoja.size;
	int *bucket = (int *)node->u.hoja.bucket;
	
	for (int i=0; i<bucketSize; i+=pageSize) {
		int n_out = min(pageSize, bucketSize-i);
		if (i > 0) {
			readFromDisk(pageId, locs, n_out);
			pageAccess++;
		}
		for (int j=0; j<n_out; ++j) {
			Tdist disTmp = dist(V[obj], locs[j], tag);
			// Tdist disTmp = dist(V[obj], V[bucket[i+j]], tag);
			addCelem(res, bucket[i+j], disTmp);
		}
		pageId++;
	}
}

static void _searchNN (vpnode *node, Obj obj, Tcelem *res)

{
	if (node == NULL) return ;
	
	// read the center object
	 int n_out = min(pageSize, node->u.hoja.size);
	 location_t *locs =  (location_t *) malloc (sizeof(location_t) * n_out);
	 readFromDisk(node->pageId, locs, n_out);
	 pageAccess++;
	 Tdist distToCenter = dist(V[obj], locs[0], tag);
	 // Tdist distToCenter = dist(V[obj], V[node->u.interno.query], tag);
	if (distToCenter <= node->u.interno.dist)
	{
		__searchBucketNN (locs, node, obj, res);
		if ((radCelem(res) == -1) ||
			(distToCenter + radCelem(res) >= node->u.interno.dist))
			_searchNN (node->u.interno.child2,obj,res);
	}
	else
	{
		_searchNN (node->u.interno.child2,obj,res);
		if ((radCelem(res) == -1) ||
			(distToCenter - radCelem(res) < node->u.interno.dist)) {
			__searchBucketNN (locs, node, obj, res);
		}
	}
	
	free(locs);
}

Tdist searchNN (Index S, Obj obj, int k, mybool show)

   { vpt *tree = (vpt*)S;
	 pageAccess = 0;
     Tdist mdif;
     Tcelem res = createCelem(k);
     _searchNN (&tree->node,obj,&res);
     if (show) showCelem (&res);
     mdif = radCelem(&res);
     freeCelem (&res);
     return mdif;
   }

static int height (vpnode *node)

   { 
     if (node == NULL) return 0;
	  return 1+height (node->u.interno.child2);
   }

static int numnodes (vpnode *node)

   { if (node == NULL) return 0;
     return 1+numnodes(node->u.interno.child2);
   }

static int numbucks (vpnode *node)

   { if (node == NULL) return 0;
     return 1 + numbucks(node->u.interno.child2);
   }

static int sizebucks (vpnode *node)

   { if (node == NULL) return 0;
     return node->u.hoja.size + sizebucks(node->u.interno.child2);
   }

static int profbucks (vpnode *node, int depth)

   { if (node == NULL) return depth;
     return depth + profbucks(node->u.interno.child2,depth+1);
   }

void prnstats (Index S)

   { vpt *tree = (vpt*)S;
     int nbucks = numbucks(&tree->node);
     int sbucks = sizebucks(&tree->node);
     int nnodes = numnodes(&tree->node);
	 printf ("number of objects: %d\n", tree->np);
     printf ("number of elements: %i\n",sbucks+nnodes);
     printf ("bucket size: %i\n",tree->bsize);
     printf ("maximum height: %i\n",height(&tree->node));
     printf ("number of nodes: %i\n",nnodes);
     printf ("number of buckets: %i\n",nbucks);
     printf ("average size of bucket: %0.2f\n",sbucks/(double)nbucks);
     printf ("average depth of bucket: %0.2f\n",
                        profbucks(&tree->node,0)/(double)nbucks);
   }
   
int countTree(Index S) {
	vpt *tree = (vpt*)S;
	int node_n = numnodes(&tree->node);
	return node_n;
}
   
long long memoryTree(Index S) {
	return (sizeof(vpnode)+dataSize) * countTree(S);	
}

