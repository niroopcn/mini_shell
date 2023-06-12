#include "main.h"

extern char shell_dir[200]; /*To display shell directory*/

void echo(char *input_string)
{
    /*Check if $ is innvoked*/
    if (!strncmp("$", input_string + 5, 1))
    {
        switch (input_string[6])
        {
        case '?':
            printf("%d\n", exit_status);
            return;
            break;
        case '$':
            printf("%d\n", getpid());
            return;
            break;
        default:
        {
            if (!strncmp("SHELL", input_string + 6, 5))
            {
                exit_status = fprintf(stderr, "%s\n", shell_dir);
                return;
            }
        }
        break;
        }
    }

    /*To display strings other than special variables*/
    execute_cmd_in_child(input_string);
}