/* 
 * Author: Matthew Johnson (CoAuthor)
 * Date: Thurs 02 Sep 2021
 * Description: 
 *   The deq class represents a DLL linked list capable of functioning
 *   as a normal DLL, stack, queue or other list implementation of adding
 *   and removing from the head/tail or any index in-between. The Node class
 *   represents each individual node of the DLL which holds a reference to a
 *   generic data point. Error messages will be prompted if elements that are
 *   requested to be removed do not exist. Likewise, if an element at an index
 *   from the head or tail doesn't exist, an error message is displayed as well. 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deq.h"
#include "error.h"

// indices and size of array of node pointers
typedef enum
{
  Head,
  Tail,
  Ends
} End;

/**
 * Node represents each individual node of the DLL
 * *np[Ends] creates a list of 2 points that point to nodes before and after it
 * data is a generic data type
 */
typedef struct Node
{
  struct Node *np[Ends]; // prev/next neighbors
  Data data;
} * Node;

/**
 * Initializes a rep object
 */
typedef struct
{
  Node ht[Ends]; // head/tail nodes
  int len;       // length of DLL
} * Rep;

/**
 * Converts a deq into a rep
 */
static Rep rep(Deq q)
{
  if (!q)
    ERROR("zero pointer");
  return (Rep)q;
}

/**
 * Inserts a new Node at the end
 */
static void put(Rep r, End e, Data d)
{
  Node newNode = malloc(sizeof(Node));
  newNode->data = d;
  newNode->np[0] = NULL;
  newNode->np[1] = NULL;

  // Check if e is the front or the back
  if (e == 0)
  { // Head
    // Check if DLL is empty
    if (r->len == 0)
    {
      r->ht[0] = newNode; // set new head
      r->ht[1] = newNode; // set new tail
    }
    else
    {
      Node head = r->ht[0]; // grab head node
      newNode->np[1] = head;
      head->np[0] = newNode;
      r->ht[0] = newNode; // sets new list head to the newNode
    }
  }
  else if (e == 1)
  { // Tail
    // Check if DLL is empty
    if (r->len == 0)
    {
      r->ht[0] = newNode;
      r->ht[1] = newNode;
    }
    else
    {
      Node tail = r->ht[1]; // grab tail node
      newNode->np[0] = tail;
      tail->np[1] = newNode;
      r->ht[1] = newNode; // sets new list tail to the newNode
    }
  }

  r->len++;
  // r->ht[Tail]++;
}

static Data ith(Rep r, End e, int i)
{
  Node head = r->ht[0];
  Node tail = r->ht[1];

  if (head == NULL || tail == NULL)
  {
    fprintf(stderr, "A NULL value was tried to be accessed");
    exit(EXIT_FAILURE);
  }

  // Starts at head
  if (e == 0)
  {
    int tmp;
    Node curNode = head;
    for (tmp = 0; tmp < i; tmp++)
    {
      if (curNode->np[1] != NULL)
      {
        curNode = curNode->np[1];
      }
      else
      {
        fprintf(stderr, "There is not a value found at the index %d requested from head", i);
        exit(EXIT_FAILURE);
      }
    }
    return curNode->data;
  }
  else
  { // Starts at tail
    int tmp;
    Node curNode = tail;
    int count = 0;
    for (tmp = 0; tmp < i; tmp++)
    {
      if (curNode->np[0] != NULL)
      {
        curNode = curNode->np[0];
        count++;
      }
      else
      {
        printf("%d", count);
        fprintf(stderr, "There is not a value found at the index %d requested from tail", i);
        exit(EXIT_FAILURE);
      }
    }
    return curNode->data;
  }
}

static Data get(Rep r, End e)
{
  if (e == 0)
  { // Head
    if (r->ht[0] != NULL)
    {
      Node newNode = r->ht[0];
      Data d = newNode->data;
      return d;
    }
    else
    {
      return NULL;
    }
  }
  else
  { // Tail
    if (r->ht[1] != NULL)
    {
      Node newNode = r->ht[1];
      Data d = newNode->data;
      return d;
    }
    else
    {
      return NULL;
    }
  }
}

static Data rem(Rep r, End e, Data d)
{
  if(r->ht[Head]->np[Head] == NULL){
    r->ht[Head] = r->ht[Head]->np[Tail];
    r->ht[Head]->np[Head] = NULL;
  } else if(r->ht[Tail]->np[Tail] == NULL){
    r->ht[Tail] = r->ht[Tail]->np[Head];
    r->ht[Tail]->np[Tail] = NULL;
  } else {
    r->ht[Head]->np[Tail] = r->ht[Tail]->np[Tail];
    r->ht[Tail]->np[Head] = r->ht[Head];
  }
  r->len--;
  return 0;
}

// Creates new deq object
extern Deq deq_new()
{
  Rep r = (Rep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->ht[Head] = 0;
  r->ht[Tail] = 0;
  r->len = 0;
  return r;
}

extern int deq_len(Deq q) { return rep(q)->len; }

extern void deq_head_put(Deq q, Data d) { put(rep(q), Head, d); }
extern Data deq_head_get(Deq q) { return get(rep(q), Head); }
extern Data deq_head_ith(Deq q, int i) { return ith(rep(q), Head, i); }
extern Data deq_head_rem(Deq q, Data d) { return rem(rep(q), Head, d); }

extern void deq_tail_put(Deq q, Data d) { put(rep(q), Tail, d); }
extern Data deq_tail_get(Deq q) { return get(rep(q), Tail); }
extern Data deq_tail_ith(Deq q, int i) { return ith(rep(q), Tail, i); }
extern Data deq_tail_rem(Deq q, Data d) { return rem(rep(q), Tail, d); }

extern void deq_map(Deq q, DeqMapF f)
{
  for (Node n = rep(q)->ht[Head]; n; n = n->np[Tail])
    f(n->data);
}

extern void deq_del(Deq q, DeqMapF f)
{
  if (f)
    deq_map(q, f);
    
  Node curr = rep(q)->ht[Head];
  while (curr)
  {
    Node next = curr->np[Tail];
    free(curr);
    curr = next;
  }
  free(q);
}

extern Str deq_str(Deq q, DeqStrF f)
{
  char *s = strdup("");
  for (Node n = rep(q)->ht[Head]; n; n = n->np[Tail])
  {
    char *d = f ? f(n->data) : n->data;
    char *t;
    asprintf(&t, "%s%s%s", s, (*s ? " " : ""), d);
    free(s);
    s = t;
    if (f)
      free(d);
  }
  return s;
}