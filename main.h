#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "errno.h"
#include <sys/wait.h>
 
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define EXT_CMDS_FILENAME "external_commands.txt"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void extract_external_commands();
void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
int check_command_type(char *command);
void *check_builtin_or_external(void *param);
//void copy_change(char *prompt, char *input_string);
void echo(char *input_string);
void execute_internal_commands();
void execute_cmd_in_child();

void register_signals();
void sigint_handler(int signum);// siginfo_t *ptr, void *data);
void signal_handler(int signum, siginfo_t *siginfo, void *data);

void convert_to_argv_format();
void free_argv();
int n_pipes(int argc, char *argv[]);

/*linked global variables*/
extern char **external_commands;
extern int global_child_pid;
extern int argc;
extern char **argv;
extern char input_string[25];
extern int exit_status; /*To update exit_status of previously running command*/
extern char shell_dir[200]; /*To display shell directory*/

#endif
