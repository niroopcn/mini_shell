#include "main.h"

/*Function to execute builtin shell commands*/
void execute_internal_commands(char *input_string)
{
    if (!strncmp("cd ", input_string, 3))
    {
        exit_status = (chdir(input_string + 3)) ? 1 : 0;

        if(exit_status)
        fprintf(stderr, "msh: cd: %s: No such file or directory\n", input_string+3);
    }
}

/*Function to execute any command in child process and collect it's exit status*/
void execute_cmd_in_child(char *input_string)
{
    /*Execute command*/
    switch (fork())
    {
    case -1:
    {
        perror("fork: Unable to create child process to execute command");
        exit(EXIT_FAILURE);
    }
    break;
    case 0:                         /*Child Process to execute command*/
        exit(system(input_string)); /*Execute command using system function and exit child process with status code*/
        break;
    default:
    {
        int status;

        /*Wait for child process to terminate*/
        waitpid(-1, &status, 0);

        /*Store exit status of child*/
        if (WIFEXITED(status))
            exit_status = WEXITSTATUS(status);

        if (WIFSIGNALED(status)) /*If Child terminated by a signal, send signal numbers*/
            exit_status = WTERMSIG(status);
    }
    break;
    }
}