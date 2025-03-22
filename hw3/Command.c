#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> // For open()

#include "Command.h"
#include "error.h"
#include <readline/history.h>

typedef struct {
  char *file;
  char **argv;
} *CommandRep;

#define BIARGS CommandRep r, int *eof, Jobs jobs
#define BINAME(name) bi_##name
#define BIDEFN(name) static void BINAME(name) (BIARGS)
#define BIENTRY(name) {#name,BINAME(name)}

static char *owd=0;
static char *cwd=0;

static void builtin_args(CommandRep r, int n) {
  char **argv=r->argv;
  for (n++; *argv++; n--);
  if (n)
    ERROR("wrong number of arguments to builtin command"); // warn
}

BIDEFN(exit) {
  builtin_args(r,0);
  *eof=1;
}

BIDEFN(history) {
  builtin_args(r,0);
  HIST_ENTRY** h_list = history_list();
  for (int i = 0; h_list[i]; i++) {
    printf("%d: %s\n", i, h_list[i]->line);
  }
  *eof=1;
}
BIDEFN(pwd) {
  builtin_args(r,0);
  if (!cwd)
    cwd=getcwd(0,0);
  printf("%s\n",cwd);
}

BIDEFN(cd) {
  builtin_args(r, 1);
  if (strcmp(r->argv[1], "-") == 0) {
    if (!owd) ERROR("No previous directory");
    char *tmp = cwd;
    cwd = owd;
    owd = tmp;
  } else {
    if (owd) free(owd);
    owd = cwd;
    cwd = strdup(r->argv[1]);
  }
  if (cwd && chdir(cwd))
    ERROR("chdir() failed");
}

static int builtin(BIARGS) {
  typedef struct {
    char *s;
    void (*f)(BIARGS);
  } Builtin;
  
  // Add new builtins here
  static const Builtin builtins[]={
    BIENTRY(exit),
    BIENTRY(pwd),
    BIENTRY(cd),
    BIENTRY(history),
    {0,0}
  };
  int i;
  for (i=0; builtins[i].s; i++)
    if (!strcmp(r->file,builtins[i].s)) {
      builtins[i].f(r,eof,jobs);
      return 1;
    }
  return 0;
}

static char **getargs(T_words words) {
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
  CommandRep r=(CommandRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->argv=getargs(words);
  r->file=r->argv[0];
  return r;
}

static void setup_redirection(CommandRep r) {
  for (int i = 0; r->argv[i]; i++) {
    if (strcmp(r->argv[i], "<") == 0) {
      int fd = open(r->argv[i + 1], O_RDONLY);
      if (fd == -1) ERROR("Failed to open input file");
      dup2(fd, STDIN_FILENO);
      close(fd);
      r->argv[i] = NULL; // Terminate arguments
      break;
    } else if (strcmp(r->argv[i], ">") == 0) {
      int fd = open(r->argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) ERROR("Failed to open output file");
      dup2(fd, STDOUT_FILENO);
      close(fd);
      r->argv[i] = NULL; // Terminate arguments
      break;
    }
  }
}

static void child(CommandRep r, int fg) {
  setup_redirection(r); // Add redirection setup
  int eof=0;
  Jobs jobs=newJobs();
  if (builtin(r,&eof,jobs))
    return;
  execvp(r->argv[0],r->argv);
  ERROR("execvp() failed");
  exit(0);
}

extern void execCommand(Command command, Pipeline pipeline, Jobs jobs,
			int *jobbed, int *eof, int fg) {
  CommandRep r=command;
  if (fg && builtin(r,eof,jobs))
    return;
  if (!*jobbed) {
    *jobbed=1;
    addJobs(jobs,pipeline);
  }
  int pid=fork();
  if (pid==-1)
    ERROR("fork() failed");
  if (pid==0)
    child(r,fg);
}

extern void freeCommand(Command command) {
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
