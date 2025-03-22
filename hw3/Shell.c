#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h> // For waitpid()

#include "Jobs.h"
#include "Parser.h"
#include "Interpreter.h"
#include "error.h"

static void wait_for_jobs(Jobs jobs) {
  while (sizeJobs(jobs) > 0) {
    wait(NULL);
    freeJobs(jobs);
  }
}

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
    if (!line)
      break;
    if (*line)
      add_history(line);
    Tree tree=parseTree(line);
    free(line);
    interpretTree(tree,&eof,jobs);
    freeTree(tree);
  }

  wait_for_jobs(jobs); // Wait for background jobs before exiting

  if (isatty(fileno(stdin))) {
    write_history(".history");
    rl_clear_history();
  } else {
    fclose(rl_outstream);
  }
  freestateCommand();
  return 0;
}
