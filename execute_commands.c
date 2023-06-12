#include "main.h"

int global_child_pid;

/*Function to execute builtin shell commands*/
void execute_internal_commands(char *input_string)
{
    if (!strncmp("cd ", input_string, 3))
    {
        exit_status = (chdir(input_string + 3)) ? 1 : 0;

        if (exit_status)
            fprintf(stderr, "msh: cd: %s: No such file or directory\n", input_string + 3);
    }
}

/*Function to execute any command in child process and collect it's exit status*/
void execute_cmd_in_child(char *input_string)
{
    /*Execute command*/
    global_child_pid = fork();
    switch (global_child_pid)
    {
    case -1:
    {
        perror("fork: Unable to create child process to execute command");
        exit(EXIT_FAILURE);
    }
    break;
    case 0: /*Child Process to execute command*/
    {
        execvp(argv[0], argv);
        //int status = system(input_string);
        //exit(status); /*Execute command using system function and exit child process with status code*/
    }
    break;
    default:
    {
        int status;
        /*Wait for child process to terminate*/
        waitpid(global_child_pid, &status, 0);

        /*Store exit status of child*/
        if (WIFEXITED(status))
            exit_status = WEXITSTATUS(status);

        if (WIFSIGNALED(status)) /*If Child terminated by a signal, store signal numbers*/
            exit_status = WTERMSIG(status);

        if(WIFSTOPPED(status))
        exit_status = WSTOPSIG(status);

        global_child_pid = 0;   /*To indicate process has finished*/
    }
    break;
    }
}

void convert_to_argv_format()
{
    argc = 0;
    char string[25] = {0};
    strcpy(string, input_string);

    const char *delimiter = " ";

    // Tokenize the input string
    char *token = strtok((char *)string, delimiter);
    while (token != NULL && argc < 20 - 1) /*20 is max args*/
    {
        // Allocate memory for the argument string
        (argv)[argc] = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if (argv[argc] == NULL)
        {
            printf("Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
        // Copy the token into the argument string
        strcpy(argv[argc], token);
        argc++;
        // Get the next token
        token = strtok(NULL, delimiter);
    }

    // Add a NULL terminator at the end of the argument array
    argv[argc] = NULL;
}

void free_argv()
{
    argc = 0;
    for(int i = 0; argv[i] != NULL; i++)
    {
        free(argv[i]);
    }
}