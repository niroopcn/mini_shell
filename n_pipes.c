#include "main.h"

int n_pipes(int argc, char *argv[])
{
    /*Check CLA Usage*/
    int pipes_count = 0, pipe_positions[50] = {0};
    if (argc > 2)
    {
        /*Last argument should not be a pipe*/
        if (strcmp("|", argv[argc - 1]))
        {
            for (int i = 0; i < argc - 1; i++) /*'|' should not exist at argv[0](first argument)*/
            {
                if (!strcmp("|", argv[i]) && !strcmp("|", argv[i + 1]))
                    return fprintf(stderr, "msh: syntax error near unexpected token '|'\n");

                /*pipes should not exist next to each other*/
                if (!strcmp("|", argv[i]) && strcmp("|", argv[i + 1]))
                {
                    argv[i] = NULL;

                    /*Store positions of pipes*/
                    pipe_positions[pipes_count++] = i;
                }
            }
        }
    }

    if (pipes_count < 1)
        return 0;

    printf("Executing pipes...\n");
    /*create 'n' pipes*/
    int p[pipes_count][2];

    for (int i = 0; i < pipes_count; i++)
        pipe(p[i]);

    int pid;
    /*Create Child-1 to execute 1st command*/
    switch (pid = fork())
    {
    case -1: /*Error in case of process creation*/
        perror("fork");
        exit(EXIT_FAILURE);
        break;
    case 0: /*Child-1 Process to execute 1st command*/
    {
        //printf("Inside Child-1: Executing ");
        //for (int i = 0; argv[i] != NULL; i++)
        //    printf("%s ", argv[i]);
        //printf("\n");

        for (int i = 0; i < pipes_count; i++)
            close(p[i][0]); /*Close all unnecessary pipes - read end*/

        for (int i = 1; i < pipes_count; i++)
            close(p[i][1]); /*Close all unnecessary pipes - write end*/

        dup2(p[0][1], 1); /*replace stdout with pipe, to send o/p of 1st command as i/p to 2nd command*/

        execvp(argv[0], argv);
    }
    break;
    default: /*Parent Process*/
    {
        /*Wait for Child-1 to terminate*/
        waitpid(pid, NULL, 0);
    }
    }

    /*Create Child-2 to Child N-1 to execute further commands*/
    for (int i = 0; i < pipes_count - 1; i++)
    {
        pid = fork();
        switch (pid)
        {
        case -1: /*Error handling while creating child process*/
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0: /*Child-2 to Child - N-1  Process to execute further commands*/
        {
            //printf("Inside Child-%d: Executing ", i + 2);
            //for (int j = pipe_positions[i] + 1; argv[j] != NULL; j++)
            //    printf("%s ", argv[j]);
            //printf("\n");

            for (int j = 0; j < pipes_count; j++)
            {
                if (j != i)
                    close(p[j][0]); /*Close all unnecessary pipes - read end except previous pipe*/
            }

            for (int j = 0; j < pipes_count; j++)
            {
                if (j != i + 1)
                    close(p[j][1]); /*Close all unnecessary pipes - write end except current pipe*/
            }

            dup2(p[i][0], 0);     /*replace stdin with pipe-1, to read i/p from previous command*/
            dup2(p[i + 1][1], 1); /*replace stdout with pipe-2, to send o/p to next command*/

            /*Start running command*/
            execvp(argv[pipe_positions[i] + 1], argv + pipe_positions[i] + 1);
        }
        break;
        default:
        {
            /*Do not wait for process to end*/
            /*Continue creating child processes or else the data won't be extracted and program will get stuck*/
        }
        break;
        }
    }

    /*Create Child-N to execute last command*/
    switch (pid = fork())
    {
    case -1: /*Error handling while creating child process*/
        perror("fork");
        exit(EXIT_FAILURE);
        break;
    case 0: /*Child-N Process to execute last command*/
    {
        //printf("Inside Final Child: Executing ");
        //for (int j = pipe_positions[pipes_count - 1] + 1; argv[j] != NULL; j++)
        //    printf("%s ", argv[j]);
        //printf("\n");

        //printf("Final output: ");
        //fflush(stdout);

        /*Close all pipes which are not necessary - SUPER IMPORTANT*/
        for (int i = 0; i < pipes_count; i++)
            close(p[i][1]);

        for (int i = 0; i < pipes_count - 1; i++)
            close(p[i][0]);

        dup2(p[pipes_count - 1][0], 0); /*replace stdin with pipe, to read i/p from 1st command*/

        /*Start running last command*/
        execvp(argv[(pipe_positions[pipes_count - 1]) + 1], argv + (pipe_positions[pipes_count - 1]) + 1);
    }
    break;
    default:
    {
        /*Close all pipes at the end*/
        for (int i = 0; i < pipes_count; i++)
        {
            close(p[i][0]);
            close(p[i][1]);
        }

        /*Wait for Child-3 to terminate*/
        waitpid(pid, NULL, 0);
    }
    break;
    }
    //sleep(1);
    return pipes_count;
}