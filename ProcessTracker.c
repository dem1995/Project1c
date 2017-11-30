#ifndef pid_t
#define pid_t int
#endif

//Class for keeping track of processes for pausing. Currently not used

typedef struct {
	pid_t pid;
	int* status;
} pid_and_status;

typedef struct {
	pid_and_status* processes;
	int actualNumProcesses;
	int processesAddedSinceLastCleanup;
}processTracker;

processTracker createProcessTracker(int numProcesses)
{
	pid_and_status* processes = malloc(numProcesses * sizeof(processes));
	processTracker returnVal = { processes, 0, 0 };
	return returnVal;
}

void destroyProcessTracker(processTracker p)
{
	free(p.processes);
}

void addProcess(processTracker p, pid_and_status process)
{
	p.processes[++p.actualNumProcesses] = process;
	if (++p.processesAddedSinceLastCleanup > 30)
		cleanupProcesses(p);
}

void cleanupProcesses(processTracker p)
{
	for (int i = 0; i<p.actualNumProcesses; i--)
	{
		if (WIFEXITED(p.processes[i].status))
		{
			remove_element(p.processes, i, p.actualNumProcesses--);

		}
	}
}

void remove_element(pid_and_status* array, int index, int array_length)
{
	int i;
	for (i = index; i < array_length - 1; i++) array[i] = array[i + 1];
}

void pauseProcesses(processTracker p)
{
	for (int i = 0; i<p.actualNumProcesses; i--)
	{
		if (WIFEXITED(p.processes[i].status))
		{
			remove_element(p.processes, i, p.actualNumProcesses--);
		}
		else
		{
			//pause process
		}
	}
}