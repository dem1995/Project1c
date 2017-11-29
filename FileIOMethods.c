#include <stdio.h>
#include <stdbool.h>
#include <string.h>
/*
*A class for defining methods that pertain to FILE manipulation
*/


/*
* Transfers the contents of inputsStream to outputStream if both are non-null FILE pointers
*/
void transferAllFileContents(FILE* inputStream, FILE* outputStream)
{
	if (inputStream != NULL && outputStream != NULL)
	{
		for (int c = fgetc(inputStream); c != EOF; c = fgetc(inputStream))
		{
			fputc(c, outputStream);
		}
	}
}

/*
*
*/
bool openFile(const char* fileDirectory, const char* fileName, FILE** filePointer, const char* streamAccessType)
{
	char* readmeFS = malloc(strlen(fileDirectory) + strlen(fileName) + 2);
	strcpy(readmeFS, fileDirectory);
	strcat(readmeFS, "/");
	strcat(readmeFS, fileName);
	*filePointer = fopen(readmeFS, streamAccessType);
	free(readmeFS);
}

/*
* Prepares user-defined input and output files and sets up pointers to them. Also starts out by clearing *inputFPPointer and *outputFPPointer if they already are defined (if they're not NULL).
*/
void setUpIO(char* inputString, char* outputString, FILE** inputFPPointer, FILE** outputFPPointer, bool shouldOpen)
{
	if (*inputFPPointer != NULL && *inputFPPointer != stdin)
	{
		fclose(*inputFPPointer);
		*inputFPPointer = stdin;
	}
	if (*outputFPPointer != NULL && outputFPPointer !=stdout)
	{
		fclose(*outputFPPointer);
		*outputFPPointer = stdout;
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