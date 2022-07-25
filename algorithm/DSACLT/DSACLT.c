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

#include "DSACLT.h"
#include "MemoryManager.h"

void prnstats (Index S);
static int pageIdx = 0;

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
   
static int _checkTree(vpnode *node) {
	if (node == NULL) return 0;
	
	int ret = 1;
	
	ret += node->u.hoja.size;
	if (node->u.hoja.size != searchbucket ((bucket)node->u.hoja.bucket,node->u.hoja.size,node->u.interno.query,node->u.interno.rc,0)) {
		return -1;
	}
	
	if (1 == node->hoja) 
	{
		if (node->u.interno.rc != node->u.interno.dist)
			return -1;
	}
	else
	{
		for (int i=0; i<ARITY_NUM; ++i) {
			if (NULL == node->u.interno.child[i])
				continue;
			int tmp = _checkTree(node->u.interno.child[i]);
			if (tmp < 0) return -1;
			ret += tmp;
			double disTmp = distance (node->u.interno.query, node->u.interno.child[i]->u.interno.query);
			if (disTmp > node->u.interno.dist) return -1;
		}
	}
	
	return ret;
}

bool checkTree(Index S) {
	sat *tree = (sat*)S;
	int n = _checkTree(tree->node);
	if (n < 0) {
		puts("Covering radius has bugs");
		return false;
	} else if (n != nV) {
		puts("bucket has bugs");
		return false;
	}
	return true;
}

static vpnode* buildsat (sat *tree, Tod *od, int nobjs)
{
	int i,per;
	Tdist mx, mn;
	vpnode* node = (vpnode *) malloc (sizeof(vpnode));;

	node->hoja = (nobjs <= tree->bsize);
	
	int j = rand() % nobjs;

	node->u.interno.query = od[j].obj;
	od[j] = od[--nobjs];
	for (i=0;i<nobjs;i++) 
	{
		od[i].dist = distance (node->u.interno.query, od[i].obj);
		od[i].tdist += od[i].dist;
	}

	qsort (od,nobjs,sizeof(Tod),compar);
	mn = od[0].dist; mx = od[nobjs-1].dist;
	node->u.interno.dist = mx;

	if (node->hoja)
	{
		node->u.interno.rc = mx;
		node->u.hoja.bucket = createbucket();
		node->u.hoja.size = 0;
		for (i=0;i<nobjs;i++) 
		{ 
			node->u.hoja.bucket = (void *) addbucket ((bucket)node->u.hoja.bucket, node->u.hoja.size, od[i].obj);
			node->u.hoja.size++;
		}
		
		for (int i=0; i<ARITY_NUM; ++i) {
			node->u.interno.child[i] = NULL;
		}
		
		// dump to File
		node->pageId = pageIdx;
		int* pids = (int *)node->u.hoja.bucket;
		assert(node->u.hoja.size <= pageSize);
		for (int i=0; i<node->u.hoja.size; i+=pageSize) {
			int n_out = min(pageSize, node->u.hoja.size-i);
			dumpToDisk(pageIdx++, pids+i, n_out);
		}
		
	} else {
		node->u.interno.rc = od[tree->bsize-1].dist;
		node->u.hoja.bucket = createbucket();
		node->u.hoja.size = 0;
		for (i=0; i<tree->bsize; i++) 
		{ 
			node->u.hoja.bucket = (void *) addbucket ((bucket)node->u.hoja.bucket, node->u.hoja.size, od[i].obj);
			node->u.hoja.size++;
		}
		
		// dump to File
		node->pageId = pageIdx;
		int* pids = (int *)node->u.hoja.bucket;
		assert(node->u.hoja.size <= pageSize);
		for (int i=0; i<node->u.hoja.size; i+=pageSize) {
			int n_out = min(pageSize, node->u.hoja.size-i);
			dumpToDisk(pageIdx++, pids+i, n_out);
		}
		
		int bt = tree->bsize, m;
		int na = max(tree->bsize, (nobjs-bt)/ARITY_NUM);
		
		for (int i=0; i<ARITY_NUM; ++i) {
			if (i == ARITY_NUM-1) {
				m = nobjs-bt;
			} else {
				m = min(na, nobjs-bt);
			}
			assert(m >= 0);
			if (m > 0) {
				node->u.interno.child[i] = buildsat (tree, od+bt, m);
			} else {
				node->u.interno.child[i] = NULL;
			}
			bt += m;
		}
	}
	

	return node;
}

   
Index build (char *dbname, int n, int maxLeafCapacity)
{ 
	sat *tree;
	int i, j;
	Tod *od;
	int nobjs = n;
	pageIdx = 0;
	
	tree = (sat *) malloc (sizeof(sat));
	tree->descr = (char *) malloc (strlen(dbname)+1);
	strcpy (tree->descr, dbname);
	tree->bsize = maxLeafCapacity;

	if (n && (n < nobjs)) nobjs = n; 

	od = (Tod *) malloc (nobjs * sizeof (Tod));

	j=0; i=0;

	while (j < nobjs) { od[i].tdist = 0;  od[i++].obj = j++;  }
	tree->node = buildsat (tree,od,nobjs);
	free (od);
	prnstats((Index)tree); 
	return (Index)tree;
}


static void freesat (vpnode *node)
{
	if (node == NULL) return ;
	freebucket ((bucket)node->u.hoja.bucket,node->u.hoja.size);
	
	if (0 == node->hoja) {
		for (int i=0; i<ARITY_NUM; ++i) {
			freesat (node->u.interno.child[i]);
			if (node->u.interno.child[i] != NULL)
				free (node->u.interno.child[i]);
		}
	} 
}

void freeIndex (Index S, mybool libobj)

   { sat *tree = (sat*)S;
     free (tree->descr);
     freesat (tree->node);
     free (tree);
	 removeFromFile(pageIdx);
   }

static void savenode (vpnode *node, int bsize, FILE *f)

   { char hoja = node->hoja ? '1' : '0';
     putc (hoja,f);
     if (node->hoja)
	{ fwrite (&node->u.hoja.size,sizeof(int),1,f);
          savebucket ((bucket)node->u.hoja.bucket,node->u.hoja.size,f);
	}
     else
	{ fwrite(&node->u.interno.query,sizeof(int), 1, f);
	  fwrite (&node->u.interno.dist, sizeof(Tdist),1,f);
	  fwrite (&node->u.interno.rc, sizeof(Tdist),1,f);
	  for (int i=0; i<ARITY_NUM; ++i) {
			savenode(node->u.interno.child[i],bsize,f);
		}
	}
   }

void saveIndex (Index S, char *fname)

   { FILE *f = fopen(fname,"w");
     sat *tree = (sat*)S;
     fwrite (tree->descr,strlen(tree->descr)+1,1,f);
     fwrite (&tree->bsize,sizeof(int),1,f);
     savenode (tree->node,tree->bsize,f);
     fclose (f);
   }

static void loadnode (vpnode *node, int bsize, FILE *f)

   { char hoja;
     hoja = getc(f);
     node->hoja = (hoja == '1');
     if (node->hoja)
	{ fread (&node->u.hoja.size,sizeof(int),1,f);
	  node->u.hoja.bucket = loadbucket (node->u.hoja.size,f);
	}
     else
	{ fread(&node->u.interno.query, sizeof(int), 1, f);
	  fread (&node->u.interno.dist,sizeof(Tdist),1,f);
	  fread (&node->u.interno.rc,sizeof(Tdist),1,f);
	   for (int i=0; i<ARITY_NUM; ++i) {
		  node->u.interno.child[i] = (svpnode *) malloc (sizeof(vpnode));
		  loadnode (node->u.interno.child[i],bsize,f);
	   }
	}
   }

Index loadIndex (char *fname)

   { char str[1024]; char *ptr = str;
     FILE *f = fopen(fname,"r");
     sat *tree = (sat *) malloc (sizeof(sat));

     while ((*ptr++ = getc(f)));
     tree->descr = (char *) malloc (ptr-str);
     strcpy (tree->descr,str);
     // tree->np=openDB (str);
     fread (&tree->bsize,sizeof(int),1,f);
     loadnode (tree->node,tree->bsize,f);
     fclose (f);
     return (Index)tree;
   }
   
static int __searchBucket (vpnode *node, Obj obj, Tdist r, mybool show) {
	int n_out = min(pageSize, node->u.hoja.size);
	if (n_out == 0) return 0;
	
	location_t *locs =  (location_t *) malloc (sizeof(location_t) * n_out);
	 
	int pageId = node->pageId, bucketSize = node->u.hoja.size;
	assert(bucketSize <= pageSize);
	assert(pageId>=0 && pageId<pageIdx);
	int *bucket = (int *)node->u.hoja.bucket;
	int rep = 0;
	
	for (int i=0; i<bucketSize; i+=pageSize) {
		int n_out = min(pageSize, bucketSize-i);
		readFromDisk(pageId, locs, n_out);
		pageAccess++;
		for (int j=0; j<n_out; ++j) {
			if (dist(V[obj], locs[j], tag) <= r) {
			// if (dist(V[obj], V[bucket[i+j]], tag) <= r) {
				rep++; if (show) printobj(bucket[i+j]);
			}
		}
		pageId++;
	}
	
	free(locs);
	return rep;
}

static int _search (vpnode *node, Obj obj, Tdist r, mybool show)
{
	if (node == NULL) return 0;
	
	Tdist rMin, rMax, lMin, lMax;
	
	int rep = 0;
	pageAccess++;
	Tdist dist;
	dist = distance (obj, node->u.interno.query); 
	rMin = dist - r, rMax = dist + r;
	
	// check current center
	if (dist <= r) { rep++; if (show) printobj(node->u.interno.query); }
	// check the bucket
	lMin = 0.0, lMax = node->u.interno.rc;
	if (dist-node->u.interno.rc <= r) {
		rep += __searchBucket (node, obj, r, show);
	}
	if (1 == node->hoja) return rep;
	
	// check the children
	rMin = dist - r, rMax = dist + r;
	lMin = node->u.interno.rc, lMax = node->u.interno.dist;
	if (rMin <= lMax && rMax >= lMin)
	{
		for (int i=0; i<ARITY_NUM; ++i) {
			if (NULL != node->u.interno.child[i])
				rep += _search(node->u.interno.child[i],obj,r,show);
		}
	}
	return rep;
}

int search (Index S, Obj obj, Tdist r, int show)

   { sat *tree = (sat*)S;
	 pageAccess = 0;
     return _search (tree->node,obj,r,show);
   }
   
static void __searchBucketNN (vpnode *node, Obj obj, Tcelem *res) {
	int n_out = min(pageSize, node->u.hoja.size);
	if (n_out == 0) return ;
	
	location_t *locs =  (location_t *) malloc (sizeof(location_t) * n_out);
	int pageId = node->pageId, bucketSize = node->u.hoja.size;
	assert(bucketSize <= pageSize);
	assert(pageId>=0 && pageId<pageIdx);
	int *bucket = (int *)node->u.hoja.bucket;
	
	for (int i=0; i<bucketSize; i+=pageSize) {
		int n_out = min(pageSize, bucketSize-i);
		readFromDisk(pageId, locs, n_out);
		pageAccess++;
		for (int j=0; j<n_out; ++j) {
			Tdist disTmp = dist(V[obj], locs[j], tag);
			// Tdist disTmp = dist(V[obj], V[bucket[i+j]], tag);
			addCelem(res, bucket[i+j], disTmp);
		}
		pageId++;
	}
	
	free(locs);
}

static void _searchNN (vpnode *node, Obj obj, Tcelem *res)

{ 
	if (node == NULL) return ;
	
	pageAccess++;
	Tdist dist;
	// check current center
	dist = distance (obj, node->u.interno.query); 
	addCelem (res,node->u.interno.query,dist);
	
	// check the bucket
	Tdist dk = radCelem(res);
	if ((dk < 0) ||
		(dist-node->u.interno.rc <= dk)) {
		__searchBucketNN(node, obj, res);
		// searchbucketNN ((bucket)node->u.hoja.bucket,node->u.hoja.size,obj,res);
	}
	
	if (1 == node->hoja) return ;
	
	// check the children
	for (int i=0; i<ARITY_NUM; ++i) {
		if (NULL == node->u.interno.child[i])
			continue;
		dk = radCelem(res);
		double rMin = dist - dk, rMax = dist + dk;
		double lMin = node->u.interno.rc, lMax = node->u.interno.dist;
		if (rMin <= lMax && rMax >= lMin)
		{
			_searchNN (node->u.interno.child[i],obj,res);
		}
	}
}

Tdist searchNN (Index S, Obj obj, int k, mybool show)

   { sat *tree = (sat*)S;
	 pageAccess = 0;
     Tdist mdif;
     Tcelem res = createCelem(k);
     _searchNN (tree->node,obj,&res);
     if (show) showCelem (&res);
     mdif = radCelem(&res);
     freeCelem (&res);
     return mdif;
   }

static int height (vpnode *node)

   {
		if (node == NULL) return 0;
	int mx = 0, tmp;
     if (node->hoja) return 1;
     else
	{ 
		for (int i=0; i<ARITY_NUM; ++i) {
			tmp = height (node->u.interno.child[i]);
			if (tmp > mx) mx = tmp;
		}
		return 1+mx;
	}
   }

static int numnodes (vpnode *node)

   { 
		if (node == NULL) return 0;
		if (node->hoja) return 0;
		
		int ret = 1;
		for (int i=0; i<ARITY_NUM; ++i) {
			ret += numnodes(node->u.interno.child[i]);
		}
		return ret;
   }

static int numbucks (vpnode *node)

   { 
		if (node == NULL) return 0;
		if (node->hoja) return 1;
		int ret = 1;
		for (int i=0; i<ARITY_NUM; ++i) {
			ret += numbucks(node->u.interno.child[i]);
		}
		return ret;
   }

static int sizebucks (vpnode *node)

   {if (node == NULL) return 0; 
	if (node->hoja) return node->u.hoja.size;
	int ret = node->u.hoja.size;
	for (int i=0; i<ARITY_NUM; ++i) {
		ret += sizebucks(node->u.interno.child[i]);
	}
	return ret;
   }

static int profbucks (vpnode *node, int depth)

   {
	if (node == NULL) return 0;	   
	if (node->hoja) return depth;
	int ret = 0;
	for (int i=0; i<ARITY_NUM; ++i) {
		ret += profbucks(node->u.interno.child[i], depth+1);
	}
	return ret;
   }

void prnstats (Index S)

   { sat *tree = (sat*)S;
     int nbucks = numbucks(tree->node);
     int sbucks = sizebucks(tree->node);
     int nnodes = numnodes(tree->node);
	 printf ("nV = %d, pageIdx = %d\n", nV, pageIdx);
     printf ("number of elements: %d\n",sbucks+nnodes);
     printf ("bucket size: %d\n",tree->bsize);
     printf ("maximum height: %d\n",height(tree->node));
     printf ("number of nodes: %d\n",nnodes);
     printf ("number of buckets: %d\n",nbucks);
     printf ("average size of bucket: %0.2f\n",sbucks/(double)nbucks);
     printf ("average depth of bucket: %0.2f\n",
                        profbucks(tree->node,0)/(double)nbucks);
   }
   
int countTree(Index S) {
	sat *tree = (sat*)S;
	int nLeaf = numbucks(tree->node);
	int nNonLeaf = numnodes(tree->node);
	return nLeaf + nNonLeaf;
}
   
long long memoryTree(Index S) {
	return sizeof(vpnode) * countTree(S);	
}

