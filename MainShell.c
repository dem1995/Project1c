﻿#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "HelperMethods.c"
#include "FileIOMethods.c"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_BUFFER 1024 // max line buffer
#define MAX_ARGS 64 // max # args
#define SEPARATORS " \t\n" // token sparators

#define KCYN  "\x1B[36m"	//Cyan text
#define RESET "\x1B[0m"		//Reset text color

bool customCommandCheck(char* arg0, char** args, FILE* readmeFP, FILE* inputFP, FILE* outputFP, char* inputFS, char* outputFS, bool ShouldAppend);

extern char** environ;
int main(int argc, char ** argv) {
	char buf[MAX_BUFFER];		// line buffer
	char* args[MAX_ARGS];		// pointers to arg strings
	char** arg;					// working pointer thru args
	char* prompt = "==>";		// shell prompt

	FILE* readmeFP = NULL;
	FILE* shellInFP = stdin;
	FILE* shellOutFP = stdout;
	FILE* inputFP = NULL;		// Pointer to the user-defined input file; is NULL if none exists
	FILE* outputFP = NULL;		// Pointer to the user-defined output file; is NULL if none exists
	bool shouldAppend = false;	// Whether the output file should be appended to (if false, it is truncated)
	
	/* Firstly, prepare the readme file pointer */
	openFile(getenv("PWD"), "readme", &readmeFP, "r");

	/* Next, see if there's a batch file to process. */
	if (argv[1]!=NULL)
	{
		if (argv[1][0] == '/')
			shellInFP = fopen(argv[1], "r");
		else
			openFile(getenv("PWD"), argv[1], &shellInFP, "r");
		if (shellInFP == NULL)
		{
			shellInFP = stdin;
			fprintf(shellOutFP, "There was a problem opening the batch file. Reverting to using standard input.");
		}
	}

	/* Now for input readin. Keep reading input until "quit" command or eof of redirected input */
	while (!feof(shellInFP)) {

		//Prints the current directory to shellOutFP
		fprintf(stdout, "Current pid %i\n", getpid());
		if(shellInFP==stdin)
			fprintf(shellOutFP, KCYN"%s"RESET"%s ", getenv("PWD"), prompt); //write prompt
		
																	//Begin the process of executing a command if the user has entered things
		if (fgets(buf, MAX_BUFFER, shellInFP)) // read a line
		{

			/*TOKENIZING THE INPUT*/
			arg = args;
			*arg++ = strtok(buf, SEPARATORS);
			while ((*arg++ = strtok(NULL, SEPARATORS))); // last entry will be NULL	

														 //if the user's input actually has things
			if (args[0])
			{

				/*HANDLING I/O*/
				char inputFS[MAX_BUFFER] = "";
				char outputFS[MAX_BUFFER] = "";
				//sets up the input file name and output file name, as well as pointers to the files those represent
				determineRedirection(args, inputFS, outputFS, &shouldAppend);
				setUpIO(inputFS, outputFS, &inputFP, &outputFP, shouldAppend);

				/*CHECKING FOR COMMANDS*/
				// check for internal commands
				if (customCommandCheck(args[0], args, readmeFP, inputFP, outputFP, inputFS, outputFS, shouldAppend))
					continue;
				// check for quitting
				else if (!strcmp(args[0], "quit")) // "quit" command
					break; // break out of 'while' look
						   //else execute command on computer
				else
				{
					fprintf(stdout, "testtest");
					forkAndLaunch(args, inputFS, outputFS, shouldAppend);
				}
			}
		}
	}

	//close files, if needed
	if (readmeFP != NULL)
		fclose(readmeFP);
	if (shellInFP != stdin)
		fclose(shellInFP);
	if (shellOutFP != stdin)
		fclose(shellOutFP);
	if (inputFP != NULL)
		fclose(inputFP);
	if (outputFP != NULL)
		fclose(outputFP);
	return 0;
}

/*
* Checks for custom commands (commands specific to this shell) and carries them out if they are found
*/
bool customCommandCheck(char* arg0, char** args, FILE* readmeFP, FILE* inputFP, FILE* outputFP, char* inputFS, char* outputFS, bool shouldAppend)
{
	if (outputFP == NULL)
	{
		outputFP = stdout;
	}

	/*CLEAR COMMAND*/
	if (!strcmp(args[0], "clr")) //"clear" command
	{
		args[0] = "clear";
		forkAndLaunch(args, inputFS, outputFS, shouldAppend);
	}

	/*DIRECTORY COMMAND*/
	else if (!strcmp(args[0], "dir"))	//"directory" command
	{
		char* dir = malloc(sizeof(char) * MAX_BUFFER);
		char* dircmdmodifier = malloc(sizeof(char) * MAX_BUFFER); //modifier for how dir should be displayed
		if (args[1] != 0)
		{
			// if the directory parameter is non-empty
			//strcpy(dircmdmodifier, "ls -al ");
			strcpy(dir, args[1]);
			//char* cmd = strcat(dircmdmodifier, dir);

			char* argv[] = { "ls", "-al", dir, 0 };

			forkAndLaunch(argv, inputFS, outputFS, shouldAppend);
			//bashLaunch(cmd);
		}
		else
		{
			// if no directory is specified, use the current directory.
			char* argv[] = { "ls", "-al", 0 };
			forkAndLaunch(argv, inputFS, outputFS, shouldAppend);
			//bashLaunch("ls -al ./");
		}
		free(dir);
		free(dircmdmodifier);
	}

	/*ENVIRON COMMAND*/
	else if (!strcmp(args[0], "environ"))
	{
		//FILE* output = fopen(outputFS, "w");

		char** env = environ;
		while (*env)
			fprintf(outputFP, "%s\n", *env++); // step through environment
	}

	/*CHANGE DIRECTORY COMMAND*/
	else if (!strcmp(args[0], "cd"))
	{
		if (args[1] == NULL) //if there's no second argument, just print the current directory
		{
			fprintf(stdout, "Current directory (According to environ): %s\n", getenv("PWD"));
		}
		else				//If there is a second argument
		{
			if (chdir(args[1]) == 0)
			{
				//Setting the environment
				char* buf = malloc(sizeof(char)*MAX_BUFFER);
				setenv("PWD", getcwd(buf, MAX_BUFFER), 1);
				free(buf);
			}
			else {
				fprintf(stdout, "There was a problem with changing directories.\n");

			}
		}
	}

	/*ECHO COMMAND*/
	else if (!strcmp(args[0], "echo"))
	{
		if (args[1] != NULL)
		{
			fprintf(outputFP, args[1]);
			for (int i = 2; args[i] != NULL; i++)
			{
				fprintf(outputFP, " ");
				fprintf(outputFP, args[i]);
			}
		}
		fprintf(outputFP, "\n");
	}

	/*PAUSE COMMAND*/
	else if (!strcmp(args[0], "pause"))
	{
		getpass("Press Enter to continue...");
	}

	/*HELP COMMAND*/
	else if (!strcmp(args[0], "help"))
	{
		transferAllFileContents(readmeFP, stdout);
	}
	/*TEST COMMAND*/
	else if (!strcmp(args[0], "test"))
	{
		while (true)
			fprintf(outputFP, "hi\n");
	}
	else
		return false;
	return true;
}







