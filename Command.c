/*
 *
 * r -> CommandRep (Command Representation), represents one Command, contains a file pointer and arguments
 * eof -> pointer to end of file, set to 1 in exit and set to 0 in child functions
 * jobs -> queue of Job objects
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <string.h>

#include "Command.h"
#include "error.h"
#include <readline/history.h>

typedef struct {
  char *file;
  char **argv;
} *CommandRep;

#define BIARGS CommandRep r, int *eof, Jobs jobs // CommandRep, End of File Pointer, Jobs
#define BINAME(name) bi_##name
#define BIDEFN(name) static void BINAME(name) (BIARGS)
#define BIENTRY(name) {#name,BINAME(name)}

static char *owd=0;
static char *cwd=0;

static void builtin_args(CommandRep r, int n) {
  // printf("builtin args\n");
  char **argv=r->argv;
  for (n++; *argv++; n--);
  if (n)
    ERROR("wrong number of arguments to builtin command"); // warn
}

BIDEFN(history){
  if(r->argv[1]){
    builtin_args(r,1);
    if (strcmp(r->argv[1],"-c")==0) {
      stifle_history(0);
      unstifle_history();
      history_base = 1;
    }
  } else {
    builtin_args(r,0);
  }

  register HIST_ENTRY **hist;
  hist = history_list ();
  int i;
  if (hist){
    for (i = 0; hist[i]; i++){
      printf ("%d: %s\n", i + history_base, hist[i]->line);
    }
  }
}

BIDEFN(exit) {
  builtin_args(r,0);
  *eof=1;
}

BIDEFN(pwd) {
  builtin_args(r,0);
  if (!cwd)
    cwd=getcwd(0,0); // returns command of the current directory
  printf("%s\n",cwd);
}

/**
 * Sets CWD to new directory and sets OWD to CWD
 * 
 * CWD stands for Current Working Directory
 * OWD stands for Old Working Directory
 */
BIDEFN(cd) {
  builtin_args(r,1);
  if (strcmp(r->argv[1],"-")==0) {
    // printf("- was found\n");
    char *twd=cwd;
    cwd=owd;
    owd=twd;
  } else {
    if (owd){
      free(owd);
    }
    owd=cwd;
    cwd=strdup(r->argv[1]); // strdup duplicates a string at CommandRep arg[1]
  }
  if (cwd && chdir(cwd))
    ERROR("chdir() failed"); // warn
}

/*
 * BuiltIn Struct:
 *  *s -> not originally set
 *  *f -> points to a list of arguments (r, eof, jobs) is what was passed in
 *  r -> CommandRep
 */
static int builtin(BIARGS) {
  // printf("BuiltIn called\n");
  typedef struct { // Builtin
    char *s;
    void (*f)(BIARGS);
  } Builtin;
  // printf("BuiltIn Struct created\n");
  static const Builtin builtins[]={
    BIENTRY(exit),
    BIENTRY(pwd),
    BIENTRY(cd),
    BIENTRY(history),
    {0,0}
  };
  // printf("BuiltIn Array created\n");

  int i;
  for (i=0; builtins[i].s; i++){ // builtins[i].s is a pointer, loop will continue until the pointer s references to a 0 or null
    // printf("\tFor loop iteration\n");
    if (!strcmp(r->file,builtins[i].s)) { // The strcmp() compares two strings character by character. If the strings are equal, the function returns 0.
      // printf("For loop is entered\n");
      builtins[i].f(r,eof,jobs); // f is a function pointer, 
      return 1;
    }
  }
  return 0;
}

static char **getargs(T_words words) {
  // printf("Get args called\n");
  int n=0;
  T_words p=words;
  while (p) {
    p=p->words;
    n++;
  }
  char **argv=(char **)malloc(sizeof(char *)*(n+1));
  if (!argv)
    ERROR("malloc() failed");
  p=words;
  int i=0;
  while (p) {
    argv[i++]=strdup(p->word->s);
    p=p->words;
  }
  argv[i]=0;
  return argv;
}

extern Command newCommand(T_words words) {
  // printf("New command\n");
  CommandRep r=(CommandRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->argv=getargs(words); // sets r->args
  r->file=r->argv[0]; // sets r->file to the first argv[0]
  return r;
}

static void child(CommandRep r, int fg) {
  int eof=0;
  Jobs jobs=newJobs();

  if (builtin(r,&eof,jobs))
    return;
  execvp(r->argv[0],r->argv);
  ERROR("execvp() failed");
  exit(0);
}

/**
 * @param command -> command being executed
 * @param pipeline -> pipeline object
 * @param jobs -> queue of jobs to be executed
 * @param jobbed -> integer pointer
 * @param eof -> end of file pointer (1 = exit)
 * @param fg -> set to 1 to run in foreground
 */
extern void execCommand(Command command, Pipeline pipeline, Jobs jobs,
			int *jobbed, int *eof, int fg) {
  CommandRep r=command;

  if (fg && builtin(r,eof,jobs)){ // error is thrown inside this builtin() function
    return;
  }
  
  if (!*jobbed) {
    *jobbed=1;
    addJobs(jobs,pipeline);
  }

  int pid=fork();
  if (pid==-1){
    ERROR("fork() failed");
  }

  if (pid==0){ // Returned a successful child process
    int rc_wait;
    rc_wait = wait(NULL);
    child(r,fg); // Passes in r (CommandRep) and fg which is set to 1 when executing (it appears)
  } else { // Returned to parent/caller
    if(fg == 1){
      wait(NULL);
    }
    // printf("parent: %d\n",(int) getpid());
  }
}

extern void freeCommand(Command command) {
  // printf("Free command\n");
  CommandRep r=command;
  char **argv=r->argv;
  while (*argv)
    free(*argv++);
  free(r->argv);
  free(r);
}

extern void freestateCommand() {
  if (cwd) free(cwd);
  if (owd) free(owd);
}
