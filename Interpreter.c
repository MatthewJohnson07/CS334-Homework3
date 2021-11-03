#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Interpreter.h"
#include "Sequence.h"
#include "Pipeline.h"
#include "Command.h"

static Command i_command(T_command t);
static void i_pipeline(T_pipeline t, Pipeline pipeline);
static void i_sequence(T_sequence t, Sequence sequence);

static Command i_command(T_command t) {
  if (!t)
    return 0;
  Command command=0;
  if (t->words)
    command=newCommand(t->words);
  return command;
}

static void i_pipeline(T_pipeline t, Pipeline pipeline) {
  if (!t)
    return;
  addPipeline(pipeline,i_command(t->command));
  i_pipeline(t->pipeline,pipeline);
}

static void i_sequence(T_sequence t, Sequence sequence) {
  if (!t)
    return;
  int fg = 1;
  if(t->op != NULL){
    // if(strcmp(t->op,"&")){ // Run in background
    //   fg = 0;
    // } else if(strcmp(t->op,";")){ // Run in foreground(?)
    //   fg = 1;
    // } else {
    //   fg = 0; 
    // }
    fg = 0;
    //printf("%s\n", t->op); // throws segmentation fault if empty
  }

  Pipeline pipeline=newPipeline(fg);
  i_pipeline(t->pipeline,pipeline);
  addSequence(sequence,pipeline);
  i_sequence(t->sequence,sequence);
}

extern void interpretTree(Tree t, int *eof, Jobs jobs) {
  if (!t)
    return;
  Sequence sequence=newSequence();
  i_sequence(t,sequence);
  execSequence(sequence,jobs,eof); 
}
