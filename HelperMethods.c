#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef pid_t
#define pid_t int
#endif

void removeRedirectionElements(char** args);
void removeString(char** stringArray, int location);

/*
* Determines the name of the user-defined input and output files, if they exist. If an output file exists, it also determines whether it should be truncated, or appended to.
*/
void determineRedirection(char** argStrings, char* inputString, char* outputString, bool* shouldAppend)
{
	bool nextStringIsInputToken = false;
	bool nextStringIsOutputToken = false;

	for (int i = 0; argStrings[i] != NULL; i++)
	{

		if (nextStringIsInputToken)
		{
			strcpy(inputString, argStrings[i]);
			nextStringIsInputToken = false;
		}

		else if (nextStringIsOutputToken)
		{
			strcpy(outputString, argStrings[i]);
			nextStringIsOutputToken = false;
		}

		if (strcmp(argStrings[i], "<") == 0)
		{
			nextStringIsInputToken = true;
		}
		else if ((strcmp(argStrings[i], ">") == 0))
		{
			nextStringIsOutputToken = true;
		}
		else if ((strcmp(argStrings[i], ">>") == 0))
		{
			nextStringIsOutputToken = true;
			*shouldAppend = true;
		}
	}

	removeRedirectionElements(argStrings);
}


/*
* Prepares user-defined input and output files and sets up pointers to them. Also starts out by clearing *inputFPPointer and *outputFPPointer if they already are defined (if they're not NULL).
*/
void setUpIO(char* inputString, char* outputString, FILE** inputFPPointer, FILE** outputFPPointer, bool shouldOpen)
{
	if (*inputFPPointer != NULL)
	{
		fclose(*inputFPPointer);
		*inputFPPointer = NULL;
	}
	if (*outputFPPointer != NULL)
	{
		fclose(*outputFPPointer);
		*outputFPPointer = NULL;
	}

	if (strcmp(inputString, "") != 0)	//if there's an input string
	{
		*inputFPPointer = fopen(inputString, "r");
		//freopen(inputString, "r", stdin);
		//stdin = fopen(inputString, "r");
		//int fd = open(inputString, O_RDONLY);
		//dup2(fd, STDIN_FILENO);
		//close(fd);
	}
	if (strcmp(outputString, "") != 0) //if there's an output string
	{
		if (shouldOpen)
			*outputFPPointer = fopen(outputString, "a");
		else
			*outputFPPointer = fopen(outputString, "w");

		//freopen(outputString, "w", stdout);
		//int fd = open(inputString, O_WRONLY | O_CREAT | O_TRUNC);
		//dup2(fd, STDOUT_FILENO);
		//close(fd);
	}
}

/*
* Forks the current process and launches a new one. Prior to the launch, however, if the user has defined their own input and output locations, redirects I/O to them.
*/
void forkAndLaunch(char** args, char* inputFS, char* outputFS, bool shouldAppend)
{
	int status;
	pid_t pid;
	switch (pid = fork())
	{
	case -1:
		//syserr("fork");
	case 0:
		if (!strcmp(inputFS, "") == 0)
			//if (inputFP!=NULL)
		{
			//dup2(fileno(inputFP), STDIN_FILENO);
			//fclose(inputFP);
			//stdin = inputFP;
			freopen(inputFS, "r", stdin);
		}
		if (!strcmp(outputFS, "") == 0)
			//if(outputFP!=NULL)
		{
			//dup2(fileno(inputFP), STDOUT_FILENO);
			//fclose(outputFP);
			//stdout = outputFP;
			if (shouldAppend)
				freopen(outputFS, "a", stdout);
			else
				freopen(outputFS, "w", stdout);
		}
		execvp(args[0], args);
		//syserr("exec");
	default:
		do
		{
			int w = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}


void bashLaunch(char* command, char* inputFS, char* outputFS, bool shouldAppend)
{
	int status;
	pid_t pid;
	switch (pid = fork())
	{
	case -1:
		//syserr("fork");
	case 0:
		if (!strcmp(inputFS, "") == 0)
			//if (inputFP!=NULL)
		{
			//dup2(fileno(inputFP), STDIN_FILENO);
			//fclose(inputFP);
			//stdin = inputFP;
			freopen(inputFS, "r", stdin);
		}
		if (!strcmp(outputFS, "") == 0)
			//if(outputFP!=NULL)
		{
			//dup2(fileno(inputFP), STDOUT_FILENO);
			//fclose(outputFP);
			//stdout = outputFP;
			if (shouldAppend)
				freopen(outputFS, "a+", stdout);
			else
				freopen(outputFS, "w", stdout);
		}
		execl("/bin/bash", "sh", "-c", command, (char *)0);
		//syserr("exec");
	default:
		do
		{
			int w = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}


/*
* Removes redirection elements from the given string array (basically if "<", ">", or ">>" are tokens in the array, it removes those and the token immediately after them in the array).
*/
void removeRedirectionElements(char** args)
{
	for (int i = 0; args[i] != NULL; i++)
	{
		if ((strcmp(args[i], "<") == 0) || (strcmp(args[i], ">") == 0) || (strcmp(args[i], ">>") == 0))
		{
			removeString(args, i);
			if (args[i] != NULL)
				removeString(args, i);
		}
	}
}

/*
* Removes a string from the array at the given index, then shifts all the elements to the right of that left one to fill the gap.
*/
void removeString(char** stringArray, int location)
{
	for (int i = location; stringArray[i] != NULL; i++)
	{
		stringArray[i] = stringArray[i + 1];
	}
}


