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

#ifndef LCLUSTER_H
#define LCLUSTER_H

#include "index.h"
#include "bucket.h"
#include "MemoryManager.h"

typedef struct svpnode
   { mybool pageId;
     struct
        { struct
             { void *bucket;
               int size;
             } hoja;
          struct
             {  Obj query;
				Tdist dist;
                struct svpnode *child2;
             } interno;
        } u;
   } vpnode;

#define child(node,i) (((Tchild*)((node)->u.interno.children))[i])

typedef struct
   { int bsize;
     char *descr;
     vpnode node;
     int np;
   } vpt;

typedef struct
   { Obj obj;
     Tdist dist,tdist;
   } Tod;

int countTree(Index S);
long long memoryTree(Index S);

#endif
