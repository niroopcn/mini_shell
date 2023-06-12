/*
Name: Niroop C Naik
Date: June 8th 2023
Description: P1 :- Minishell
Features:
1. Prompt user for command, customizable prompt using environmental variable PS1; ex: PS1=newprompt
2. Execute the command entered by the user
3. Special Variables: echo $$, echo $? and echo $SHELL
4. Signal handling: SIGINT(Ctrl + C), SIGTSTP(Ctrl + Z)
5. Built-in commands: cd, exit
6. Pipe functionality: ls -l | grep a | wc
*/

#include "main.h"

/*Initialize character arrays to store prompt and input string*/
char prompt[25] = "msh", input_string[25] = {0};
char shell_dir[200]; /*To display shell directory*/
char cwd[200];		 /*To store current Working Directory*/
int exit_status;	 /*To collect exit status of previously running command*/
int argc;
char **argv;

int main()
{
	/* Allocate memory for the argument array */
	argc = 0;									 /* Argument count */
	argv = (char **)malloc(20 * sizeof(char *)); /*20 is max args*/
	if (argv == NULL)
	{
		printf("Memory allocation failed!\n");
		exit(EXIT_FAILURE);
	}

	/*Signal handling for SIGINT(Ctrl + C), SIGTSTP(Ctrl + Z)*/
	register_signals();

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
		convert_to_argv_format(); /*Convert string to argv format*/

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
			/*Check if pipe are passed and execute*/
			if (n_pipes(argc, argv) != 0)
			{
				break;	/*break out of switch*/
			}

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
		}
		free_argv();
	}
}

/*To handle signal to terminate a process*/
void sigint_handler(int signum) //, siginfo_t *ptr, void *data)
{
	printf("\n");
	if (signum == SIGINT)
	{
		if (global_child_pid == 0)
		{
			getcwd(cwd, 200); /*get current working directory to display with prompt*/
			printf(ANSI_COLOR_GREEN "%s:%s$ " ANSI_COLOR_RESET, prompt, cwd);
			fflush(stdout);
		}
		else
		{
			kill(SIGINT, global_child_pid);
			printf("Process %d Terminated\t (%s)\n", global_child_pid, input_string);
			global_child_pid = 0;
		}
	}
}

/*To handle signal to stop a process*/
void signal_handler(int signum, siginfo_t *siginfo, void *data)
{
	if (signum == SIGTSTP)
	{
		if (global_child_pid != 0)
		{
			kill(global_child_pid, SIGTSTP);
			printf("\nProcess %d Stopped\t (%s)\n", global_child_pid, input_string);
			global_child_pid = 0;
		}
	}
	if(signum == SIGCHLD)
	{

	}
}

void register_signals()
{
	/*Register SIGINT(Ctrl + C) Signal*/
	signal(SIGINT, sigint_handler);

	/*Register SIGTSTP(Ctrl + Z) Signal*/
	struct sigaction act_sigtstp;
	memset(&act_sigtstp, 0, sizeof(act_sigtstp)); /*Clear memory*/
	act_sigtstp.sa_sigaction = signal_handler;	  /*Assign signal handler*/
	act_sigtstp.sa_flags = SA_SIGINFO;			  /*Assign flags*/
	sigaction(SIGTSTP, &act_sigtstp, NULL);

	/*Not yet Implemented - Register SIGCHLD for cleaning resources for background tasks*/
	//struct sigaction act_sigchld;
	//memset(&act_sigchld, 0, sizeof(act_sigchld));	  /*Clear memory*/
	//act_sigchld.sa_sigaction = signal_handler;		  /*Assign signal handler*/
	//act_sigchld.sa_flags = SA_SIGINFO | SA_NOCLDWAIT; /*Assign flags*/
	//sigaction(SIGCHLD, &act_sigchld, NULL);
}