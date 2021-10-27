#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Jobs.h"
#include "Parser.h"
#include "Interpreter.h"
#include "error.h"

int main() {
  int eof=0;
  Jobs jobs=newJobs();
  char *prompt=0;

  if (isatty(fileno(stdin))) {
    using_history();
    read_history(".history");
    prompt="$ ";
  } else {
    rl_bind_key('\t',rl_insert);
    rl_outstream=fopen("/dev/null","w");
  }
  
  while (!eof) {
    char *line=readline(prompt);
    // printf("%s\n",line); // prints the line as is, ex. pwd would print pwd
    if (!line){
      break;
    }

    if (*line){
      add_history(line); // adds history to the end of the history list
    }
    Tree tree=parseTree(line);
    free(line);
    interpretTree(tree,&eof,jobs); // Interpreter
    freeTree(tree);
  }

  if (isatty(fileno(stdin))) {
    write_history(".history");
    rl_clear_history();
  } else {
    fclose(rl_outstream);
  }
  freestateCommand();
  return 0;
}
