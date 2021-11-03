#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Pipeline.h"
#include "deq.h"
#include "error.h"

typedef struct {
  Deq processes;
  int fg;			// not "&"
} *PipelineRep;

extern Pipeline newPipeline(int fg) {
  PipelineRep r=(PipelineRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->processes=deq_new();
  r->fg=fg;
  return r;
}

extern void addPipeline(Pipeline pipeline, Command command) {
  PipelineRep r=(PipelineRep)pipeline; // Creates pipeline object
  deq_tail_put(r->processes,command); // Inserts, another queue of processes as one element of the array
}

extern int sizePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  return deq_len(r->processes);
}

static void execute(Pipeline pipeline, Jobs jobs, int *jobbed, int *eof) {
  // printf("Execute called\n");
  PipelineRep r=(PipelineRep)pipeline;

  for (int i=0; i<sizePipeline(r) && !*eof; i++){
    if(r->fg == 1){ // TODO CHANGE BACK TO == 1
      execCommand(deq_head_ith(r->processes,i),pipeline,jobs,jobbed,eof,1); // Processes is a queue, uses head_ith to get i from queue
    } else {
      // printf("fg is 0\n");
      execCommand(deq_head_ith(r->processes,i),pipeline,jobs,jobbed,eof,0); 
    }
    
  }
}

extern void execPipeline(Pipeline pipeline, Jobs jobs, int *eof) {
  // printf("ExecPipeline called\n");
  int jobbed=0;

  execute(pipeline,jobs,&jobbed,eof);
  if (!jobbed)
    freePipeline(pipeline);	// for fg builtins, and such
}

extern void freePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  deq_del(r->processes,freeCommand);
  free(r);
}
