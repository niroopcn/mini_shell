/*
Name: Niroop C Naik
Date: June 8th 2023
Description: P1 :- Minishell
*/

#include "main.h"

/*Initialize character arrays to store prompt and input string*/
char prompt[25] = "msh", input_string[25] = {0};

char shell_dir[200]; /*To display shell directory*/
char cwd[200];		 /*To store current Working Directory*/
int exit_status;	 /*To collect exit status of previously running command*/
extern char **external_commands;

int main()
{
	signal(SIGINT, signal_handler);

	getcwd(shell_dir, 200);		 /*Store $SHELL binary directory*/
	extract_external_commands(); /*Extract external command list from a file*/

	/*Display prompt, scan input and execute commands*/
	scan_input(prompt, input_string);

	return 0;
}

/*Function to display prompt, scan input and execute commands*/
void scan_input(char *prompt, char *input_string)
{
	while (1)
	{
		/*Display the prompt*/
		getcwd(cwd, 200); /*get current working directory to display with prompt*/
		printf(ANSI_COLOR_GREEN "%s:%s$ " ANSI_COLOR_RESET, prompt, cwd);
		fflush(stdout);

		memset(input_string, 0, 25);	/*Clear input string*/
		scanf("%[^\n]s", input_string); /*Read input from user*/
		while (getchar() != '\n')
			;

		/*Code to customise prompt*/
		if (!strncmp("PS1=", input_string, 4))
		{
			if (input_string[4] != ' ')
			{
				strcpy(prompt, input_string + 4); /*Copy the modified prompt*/
				memset(input_string, 0, 25);	  /*Clear input string*/
				continue;						  /*Loop again to display prompt*/
			}
		}

		/*If ' ' is present or Enter is pressed, break and display prompt again*/
		switch (input_string[0])
		{
		case '\0':
		case '\n':
		case ' ':
			break;
		default:
		{
			/*get command passed*/
			char *cmd = get_command(input_string);
			// printf("cmd %s\n", cmd);

			/*Check command if internal or external or doesn't exist*/
			int cmd_type = check_command_type(cmd);
			// printf("cmd_type %d\n", cmd_type);

			if (cmd_type == NO_COMMAND)
			{
				/*To run executables*/
				if (!strncmp("./", input_string, 2))
					execute_cmd_in_child(input_string);
				else
					fprintf(stderr, "%s: No such command\n", cmd);
			}
			else if (cmd_type == BUILTIN)
			{
				/*Check if exit command is issued*/
				if (!strncmp(input_string, "exit", 4))
					return; /*return control back to main function and terminate program*/

				execute_internal_commands(input_string);
			}
			else /*External commands*/
			{
				if (!strncmp(cmd, "echo", 4))
					echo(input_string);
				else
					execute_cmd_in_child(input_string);
			}
		}
		break;
		}
	}
}

void signal_handler(int signum) //, siginfo_t *ptr, void *data)
{
	printf("\n");
	if (signum == SIGINT)
	{
		getcwd(cwd, 200); /*get current working directory to display with prompt*/
		printf(ANSI_COLOR_GREEN "%s:%s$ " ANSI_COLOR_RESET, prompt, cwd);
		fflush(stdout);
	}
}