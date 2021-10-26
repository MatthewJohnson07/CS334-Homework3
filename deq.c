#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deq.h"
#include "error.h"

// indices and size of array of node pointers
typedef enum {Head,Tail,Ends} End;

typedef struct Node {
  struct Node *np[Ends];		// next/prev neighbors
  Data data;
} *Node;

typedef struct {
  Node ht[Ends];			// head/tail nodes
  int len;
} *Rep;

static Rep rep(Deq q) {
  if (!q) ERROR("zero pointer");
  return (Rep)q;
}

/**
 * Using this method, you can append an item to the beginning or end of a list at the head or tail position.
 * @param r The list appending to
 * @param e The head or tail enum
 * @param d The data being added
 * @return void
 * 
 */
static void put(Rep r, End e, Data d) 
{
  //This section adds at the tail
  if(e==Tail)
  {
    Node start = malloc(sizeof(struct Node));
    memset(start, 0, sizeof(*start));
    start->data=d;
    if(r->len==0)
    {
      r->ht[Tail]=start;
      r->ht[Head]=start;
    }
    else
    {
      Node prevTail = r->ht[Tail];
      r->ht[Tail]=start;
      prevTail->np[Tail]=start;
      start->np[Head]=prevTail;
    }
    r->len=r->len+1;
  }
  //This section adds at the head
  if(e==Head)
  {
    Node start = malloc(sizeof(struct Node));
    memset(start, 0, sizeof(*start));
    start->data=d;
    if(r->len==0)
    {
      r->ht[Head]=start;
      r->ht[Tail]=start;
    }
    else
    {
      Node prevHead = r->ht[Head];
      r->ht[Head]=start;
      prevHead->np[Head]=start;
      start->np[Tail]=prevHead;
    }
    r->len=r->len+1;
  }
}

/**
 * This method returns the data from a desired index while leaving the list unchanged.
 * @param r The list being parsed
 * @param e The head or tail enum
 * @param i The desired index
 * @return data
 * 
 */
static Data ith(Rep r, End e, int i) 
{ 
  //covers out of bounds index
  if(i<0 || i >= r->len)
  {
    printf("The desired index is not in the list.\n");
    return NULL;
  }
  //this section covers starting at the tail
  if(e==Tail)
  {
    Node req=r->ht[Tail];
    for(int k=0; k!=i; k++)
    {
      req=req->np[Head];
    }
    return req->data;
  }
  //this section covers starting at the head
  if(e==Head)
  {
    Node req=r->ht[Head];
    for(int k=0; k!=i; k++)
    {
      req=req->np[Tail];
    }
    return req->data;
  }
  return 0; 
}

/**
 * This method removes a node from the beginning or end of the desired list.
 * @param r The list appending from
 * @param e The head or tail enum
 * @return data
 * 
 */
static Data get(Rep r, End e) 
{
    if(r->len > 2)
    {
      Data d=r->ht[e]->data;
      int i = 0;
      if(e==Head)
      {
        i = 1;
      }
      Node nxpvNode = r->ht[e]->np[i];
      nxpvNode->np[e]=NULL;
      free(r->ht[e]);
      r->ht[e]=nxpvNode;
      r->len=r->len-1;
      return d;
    }
    if(r->len == 2)
    {
      Data d=r->ht[e]->data;
      int i = 0;
      if(e==Head)
      {
        i = 1;
      }
      Node nxpvNode = r->ht[e]->np[i];
      nxpvNode->np[Head]=NULL;
      nxpvNode->np[Tail]=NULL;
      free(r->ht[e]);
      r->ht[Head]=nxpvNode;
      r->ht[Tail]=nxpvNode;
      r->len=r->len-1;
      return d;
    }
    if(r->len == 1)
    {
      Node back=r->ht[e];
      r->ht[Head]=NULL;
      r->ht[Tail]=NULL;
      free(r->ht[e]);
      r->len=r->len-1;
      return back->data;
    }
  //case for empty list
  if(r->len==0)
  {
    printf("List is empty, can't remove.\n");
    return NULL;
  }
  return 0;
}

/**
 * This method removes specified data if it is found in the desired list.
 * @param r The desired list
 * @param e The head or tail enum
 * @param d The specified data to be found
 * @return data
 * 
 */
static Data rem(Rep r, End e, Data d) 
{
  //Takes care of Tail
  if(e==Tail)
  {
    Node pos=r->ht[Tail];
    while(pos != NULL)
    {
      //This section covers if the data is equal
      if(pos->data == d)
      {
        if(pos==r->ht[Head])
        {
          return get(r,Head);
        }
        else if(pos==r->ht[Tail])
        {
          return get(r,Tail);
        }
        else
        {
          Data rem_data=pos->data;
          
          Node prevNode = pos->np[Head];
          Node nextNode = pos->np[Tail];

          prevNode->np[Tail]=nextNode;
          nextNode->np[Head]=prevNode;
          free(pos);
          r->len=r->len-1;
          return rem_data;
        }
      }
      //this section covers if data isn't equal
      else
      {
        if(pos==r->ht[Head])
        {
          printf("List does not contain this data.\n");
          return NULL;
          break;
        }
        else
        {
          pos=pos->np[Head];
        }
      }
    }
  }
  //Takes care of Head
  if(e==Head)
  {
    Node pos=r->ht[Head];
    while(pos != NULL)
    {
      //This section covers if the data is equal
      if(pos->data == d)
      {
        if(pos==r->ht[Head])
        {
          return get(r,Head);
        }
        else if(pos==r->ht[Tail])
        {
          return get(r,Tail);
        }
        else
        {
          Data rem_data=pos->data;
          
          Node prevNode = pos->np[Head];
          Node nextNode = pos->np[Tail];

          prevNode->np[Tail]=nextNode;
          nextNode->np[Head]=prevNode;
          free(pos);
          r->len=r->len-1;
          return rem_data;
        }
      }
      //this section covers if data isn't equal
      else
      {
        if(pos==r->ht[Tail])
        {
          printf("List does not contain this data.\n");
          return NULL; 
          break;
        }
        else
        {
          pos=pos->np[Tail];
        }
      }
    }
  }
  //Takes care of empty list
  if(r->len==0)
  {
    printf("List is empty, data cannot be removed.\n");
    return NULL;
  }
  //this section only returns in the break is activated.
  return NULL;
}

extern Deq deq_new() {
  Rep r=(Rep)malloc(sizeof(*r));
  if (!r) ERROR("malloc() failed");
  r->ht[Head]=0;
  r->ht[Tail]=0;
  r->len=0;
  return r;
}

extern int deq_len(Deq q) { return rep(q)->len; }

extern void deq_head_put(Deq q, Data d) {        put(rep(q),Head,d); }
extern Data deq_head_get(Deq q)         { return get(rep(q),Head); }
extern Data deq_head_ith(Deq q, int i)  { return ith(rep(q),Head,i); }
extern Data deq_head_rem(Deq q, Data d) { return rem(rep(q),Head,d); }

extern void deq_tail_put(Deq q, Data d) {        put(rep(q),Tail,d); }
extern Data deq_tail_get(Deq q)         { return get(rep(q),Tail); }
extern Data deq_tail_ith(Deq q, int i)  { return ith(rep(q),Tail,i); }
extern Data deq_tail_rem(Deq q, Data d) { return rem(rep(q),Tail,d); }

extern void deq_map(Deq q, DeqMapF f) {
  for (Node n=rep(q)->ht[Head]; n; n=n->np[Tail])
    f(n->data);
}

extern void deq_del(Deq q, DeqMapF f) {
  if (f) deq_map(q,f);
  Node curr=rep(q)->ht[Head];
  while (curr) {
    Node next=curr->np[Tail];
    free(curr);
    curr=next;
  }
  free(q);
}

extern Str deq_str(Deq q, DeqStrF f) {
  char *s=strdup("");
  for (Node n=rep(q)->ht[Head]; n; n=n->np[Tail]) {
    char *d=f ? f(n->data) : n->data;
    char *t; asprintf(&t,"%s%s%s",s,(*s ? " " : ""),d);
    free(s); s=t;
    if (f) free(d);
  }
  return s;
}