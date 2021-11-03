/* 
 * Author: Matthew Johnson (CoAuthor)
 * Date: Tues 02 Nov 2021
 * Description: 
 *   The parser class has several useful methods utilized by shell.c in order to produce
 *   a tree for a line typed in the shell prompt. The main function, parseTree() utilizes 
 *   many of the helper methods to scan through each token and store in a T_words object
 *   while appropriately identifying operators such as '>' '<' '&' ';'. Within a sequence
 *   object, the operator is stored using ->op which is referenced inside the Interpreter. 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Tree.h"
#include "Scanner.h"
#include "error.h"

static Scanner scan;

#undef ERROR
#define ERROR(s) ERRORLOC(__FILE__,__LINE__,"error","%s (pos: %d)",s,posScanner(scan))

static char *next()       { return nextScanner(scan); }
static char *curr()       { return currScanner(scan); }
static int   cmp(char *s) { return cmpScanner(scan,s); }
static int   eat(char *s) { return eatScanner(scan,s); }

static T_word p_word();
static T_words p_words();
static T_command p_command();
static T_pipeline p_pipeline();
static T_sequence p_sequence();

static T_word p_word() {
  char *s=curr();
  if (!s)
    return 0;
  T_word word=new_word();
  word->s=strdup(s);
  next();
  return word;
}

static T_words p_words() {
  //printf("cur: %s\n", curr());
  T_word word=p_word();
  if (!word)
    return 0;
  T_words words=new_words();
  words->word=word;
  if (cmp("|") || cmp("&") || cmp(";"))
    return words;
  words->words=p_words();
  return words;
}

static T_command p_command() {
  T_words words=0;
  words=p_words();
  if (!words)
    return 0;
  T_command command=new_command();
  command->words=words;
  return command;
}

/**
 * Creates a new command, calling p_command()
 * Creates a new pipeline, calling new_pipeline
 */
static T_pipeline p_pipeline() {
  T_command command=p_command();
  if (!command)
    return 0;
  
  T_pipeline pipeline=new_pipeline();
  pipeline->command=command;
  if (eat("|"))
    pipeline->pipeline=p_pipeline();
  if (eat(">")){
    // TODO
  }
  if (eat("<")){
    // TODO
  }

  return pipeline;
}

/**
 * Creates a pipeline object by calling p_pipeline()
 * Creates a new sequence object calling new_sequence()
 * 
 * Eats '&' and ';', and expects another p_sequence()
 */
static T_sequence p_sequence() {
  T_pipeline pipeline=p_pipeline();
  if (!pipeline)
    return 0;
  T_sequence sequence=new_sequence();
  sequence->pipeline=pipeline;
  // printf("%s", curr()); // Prints & or last character of line not already processed
  if (eat("&")) {
    sequence->op="&"; // Stores inside sequence, later referenced in Interpreter.c
    sequence->sequence=p_sequence();
  }
  if (eat(";")) {
    sequence->op=";";
    sequence->sequence=p_sequence();
  }
  // printf("current %s\n", curr()); 
  return sequence;
}

/**
 * Creates scanner using a pointer (passed as parameter)
 * After p_sequence() is called and executed, if there is
 * any characters left in the line, it will throw an error. 
 * 
 * *s is the line to be parsed
 */
extern Tree parseTree(char *s) { // Called from shell.c, returns tree
  scan=newScanner(s);
  Tree tree=p_sequence();
  if (curr())
    ERROR("extra characters at end of input");
  freeScanner(scan);
  return tree;
}

static void f_word(T_word t);
static void f_words(T_words t);
static void f_command(T_command t);
static void f_pipeline(T_pipeline t);
static void f_sequence(T_sequence t);

static void f_word(T_word t) {
  if (!t)
    return;
  if (t->s)
    free(t->s);
  free(t);
}

static void f_words(T_words t) {
  if (!t)
    return;
  f_word(t->word);
  f_words(t->words);
  free(t);
}

static void f_command(T_command t) {
  if (!t)
    return;
  f_words(t->words);
  free(t);
}

static void f_pipeline(T_pipeline t) {
  if (!t)
    return;
  f_command(t->command);
  f_pipeline(t->pipeline);
  free(t);
}

static void f_sequence(T_sequence t) {
  if (!t)
    return;
  f_pipeline(t->pipeline);
  f_sequence(t->sequence);
  free(t);
}

extern void freeTree(Tree t) {
  f_sequence(t);
}
