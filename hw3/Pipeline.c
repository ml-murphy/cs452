#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "Pipeline.h"
#include "deq.h"
#include "error.h"
#include "Command.h"

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
  PipelineRep r=(PipelineRep)pipeline;
  deq_tail_put(r->processes,command);
}

extern int sizePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  return deq_len(r->processes);
}

static void execute(Pipeline pipeline, Jobs jobs, int *jobbed, int *eof) {
  PipelineRep r = (PipelineRep)pipeline;
  int pipefd[2], in_fd = STDIN_FILENO;

  for (int i = 0; i < sizePipeline(r) && !*eof; i++) {
    Command command = deq_head_ith(r->processes, i);

    // Check if the command is a builtin and execute it in the parent process
    if (i == sizePipeline(r) - 1 && builtin(command, eof, jobs)) {
      continue; // Skip forking if it's a builtin
    }

    if (i < sizePipeline(r) - 1) {
      if (pipe(pipefd) == -1) ERROR("pipe() failed");
    }

    int pid = fork();
    if (pid == -1) ERROR("fork() failed");
    if (pid == 0) {
      if (i > 0) {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
      }
      if (i < sizePipeline(r) - 1) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
      }
      execCommand(command, pipeline, jobs, jobbed, eof, 1);
      exit(0);
    }

    if (i > 0) close(in_fd);
    if (i < sizePipeline(r) - 1) {
      close(pipefd[1]);
      in_fd = pipefd[0];
    }
  }

  while (wait(NULL) > 0); // Wait for all children
}

extern void execPipeline(Pipeline pipeline, Jobs jobs, int *eof) {
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
