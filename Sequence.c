#include "Sequence.h"
#include "deq.h"
#include "error.h"

extern Sequence newSequence() { // Sequence is another queue of Sequences
  // printf("NewSequence called\n");
  return deq_new();
}

extern void addSequence(Sequence sequence, Pipeline pipeline) {
  // printf("AddSequence called\n");
  deq_tail_put(sequence,pipeline);
}

extern void freeSequence(Sequence sequence) {
  // printf("FreeSequence called\n");
  deq_del(sequence,freePipeline);
}

extern void execSequence(Sequence sequence, Jobs jobs, int *eof) {
  // printf("ExecSequence called\n");
  while (deq_len(sequence) && !*eof){ // While length of the sequence queue is not 0 and pointer of EOF is not 0
    execPipeline(deq_head_get(sequence),jobs,eof); // Gets top item from sequence, passes in Jobs queue, and EOF pointer
  }

  freeSequence(sequence);
}
