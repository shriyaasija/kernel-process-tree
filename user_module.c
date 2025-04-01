#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_CHILDREN 3

void print_memory_map(pid_t pid) {
	char cmd[256];
	snprintf(cmd, sizeof(cmd), "cat /proc/%d/maps", pid);
	FILE *fp = popen(cmd, "r");
	char line[256];
	printf("\nMemory map of process %d:\n", pid);
	printf("----------------------------\n");
	
	while (fgets(line, sizeof(line), fp)) {
        	// Check for stack region (usually ends with 'stack')
		if (strstr(line, "[stack]")) {
		    printf("Stack Region: %s\n\n", line);
		}
		// Check for heap region (usually named 'heap')
		else if (strstr(line, "[heap]")) {
		    printf("Heap Region: %s\n\n", line);
		}
    	}
    	
	printf("\n");
}

int main() {
	pid_t parent_pid = getpid();
	printf("Parent process: PID %d\n", parent_pid);
	
	FILE *fp = fopen("parent_pid.txt", "w");
    	if (fp) {
        	fprintf(fp, "%d\n", parent_pid);
        	fclose(fp);
    	}
	
	for(int i = 0; i < NUM_CHILDREN; ++i) {
		sleep(10);
		pid_t pid = fork();
		
		if(pid < 0) {
			perror("Fork failed");
			exit(EXIT_FAILURE);
		}
		
		if(pid == 0){
			void *mem = malloc(1024 * 1024);
			if(!mem) {
				perror("Memory allocation failed");
				exit(EXIT_FAILURE);
			}
			
			printf("Child process %d created (Parent: %d)\n", getpid(), getppid());
			print_memory_map(getpid());
			sleep(120);
			free(mem);
			exit(EXIT_SUCCESS);
		}
	}
	
	//Parent waits for all children to terminate
	for(int i = 0; i < NUM_CHILDREN; ++i){
		wait(NULL);
	}
	
	printf("All child processes finished.\n");
	printf("Parent process (PID = %d) finished.\n", getpid());

	
	return 0;
}
