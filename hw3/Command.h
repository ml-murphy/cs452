#ifndef COMMAND_H
#define COMMAND_H

typedef struct {
    char *file;
    char **argv;
} *CommandRep;

typedef void *Command;

#include "Tree.h"
#include "Jobs.h"
#include "Sequence.h"

#define BIARGS CommandRep r, int *eof, Jobs jobs

extern Command newCommand(T_words words);

extern void execCommand(Command command, Pipeline pipeline, Jobs jobs,
			int *jobbed, int *eof, int fg);

extern int builtin(BIARGS);

extern void freeCommand(Command command);
extern void freestateCommand();

#endif
