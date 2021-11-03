/* 
 * Author: Matthew Johnson (CoAuthor)
 * Date: Tues 02 Nov 2021
 * Description: 
 *   This file is where everything begins. parseTree returns essentially a Sequence object
 *   that stores the words of a command as well as any operators. With this, the tree can
 *   be passed into an Interpreter and converted back into a Sequence to utilize useful data. 
 *   Within this function, the tree is created and individual commands are executed using other
 *   classes and the next prompt is displayed within this class. 
 * 
 */

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
