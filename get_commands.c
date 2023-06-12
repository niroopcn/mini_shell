#include "main.h"

/*Structure to be used in pthreads for searching internal or external command*/
typedef struct
{
    char **cmd_list;
    char *cmd;
    int type;
} data_t;

int builtin_or_external;   /*Default value set as NO_COMMAND*/

/*Initialize an array of character pointers to store names of all shell builtin commands*/
char *builtins[] = {"alias", "bg", "bind", "break", "builtin", "case", "cd", "command", "compgen", "complete", "continue", "declare",
                    "dirs", "disown", "echo", "enable", "eval", "exec", "exit", "export", "fc", "fg", "getopts", "hash",
                    "help", "history", "if", "jobs", "kill", "let", "local", "logout", "popd", "printf", "pushd", "pwd", "read",
                    "readonly", "return", "set", "shift", "shopt", "source", "suspend", "test", "times", "trap", "type",
                    "typeset", "ulimit", "umask", "unalias", "unset", "until", "wait", "while", NULL};

char **external_commands;

/*Function to extract command name*/
char *get_command(char *input_string)
{
    char *command = malloc(25 * sizeof(char));
    int i;
    for (i = 0; (input_string[i] != ' ') && (input_string[i] != '\0'); i++)
    {
        command[i] = input_string[i];
    }
    command[i] = '\0';

    return command;
}

/*Funcion to extract external command names from file*/
void extract_external_commands()
{
    /*Append names of external commands to a 2d array*/
    external_commands = calloc(200, sizeof(char *));

    if (external_commands == NULL)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    int fd = open(EXT_CMDS_FILENAME, O_RDONLY); /*Open file in Read Mode*/
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char ch, buffer[25] = {'\0'}; /*Temporary buffer*/
    int i = 0, j = 0;

    while (read(fd, &ch, 1) > 0)
    {
        /*Read characters and store them in buffer temporarily*/
        if (ch != '\n')
            buffer[i++] = ch;
        else /*When end of string is reached, copy that buffer to external commands 2d array*/
        {
            /*Allocate memory for string in 2d array*/
            (external_commands)[j] = calloc(strlen(buffer) + 1, sizeof(char));
            if ((external_commands)[j] == NULL)
            {
                perror("calloc");
                exit(EXIT_FAILURE);
            }
            strcpy((external_commands)[j], buffer);
            j++;
            i = 0;
            memset(buffer, 0, sizeof(buffer));
        }
    }
}

/*Function to check if command is external or internal*/
int check_command_type(char *command)
{
    /*Create threads to check commands simultaneously*/
    pthread_t tid_internal, tid_external;

    data_t data1, data2;

    data1.cmd_list = builtins;
    data1.cmd = command;
    data1.type = BUILTIN;

    data2.cmd_list = external_commands;
    data2.cmd = command;
    data2.type = EXTERNAL;

    builtin_or_external = NO_COMMAND;

    pthread_create(&tid_internal, NULL, check_builtin_or_external, &data1); /*create thread*/
    
    pthread_create(&tid_external, NULL, check_builtin_or_external, &data2); /*create thread*/

    pthread_join(tid_internal, NULL);
    pthread_join(tid_external, NULL);

    return builtin_or_external;
}

/*Function to assign BUILTIN or EXTERNAL macro ad type of command*/
void *check_builtin_or_external(void *param)
{
    data_t *data = param;

    /*Check if command is internal or external*/
    for (int i = 0; (data->cmd_list[i] != NULL); i++)
    {
        //printf("inside %s %d\n", data->cmd_list[i], data->type);
        if (!strcmp(data->cmd, data->cmd_list[i]))
        {
            builtin_or_external = data->type;   /*Store type of command in global variable*/
            break;
        }
    }
}