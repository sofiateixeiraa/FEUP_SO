#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>



void buildPipename(char* pipename, int x, int y){
    char aux[12] = "pipe";
    char aux2[3] = "to";
	char int_str[3];
	char int_str2[3];
	sprintf(int_str, "%d", x);
	sprintf(int_str2, "%d", y);
	strcat(aux,int_str);
	strcat(aux,aux2);
	strcat(aux,int_str2);
    
    memset(pipename, '\0', sizeof(pipename)); //clear string
    strcpy(pipename, aux);
}


int main(int argc, char* argv[]){
	    pid_t pid = 0;
    	int token = 0;
        int child = 0;
        int readPipe = 0;
        int writePipe = 0;
        char pipename[12];
        pid_t process_id = -1;
        float xr = 0;

	    if(argc < 4){
        	fprintf(stderr, "Not enough arguments.\ntokenring <number of processes> <probability> <seconds>\nExample: tokenring 5 0.01 10\n");
        	exit(EXIT_FAILURE);
    	}	
    
    	int n = atoi(argv[1]);
    	if(n < 2){
        	fprintf(stderr, "Number of processes must be an integer greather than 1.\n");
        	exit(EXIT_FAILURE);
    	}

        float p = atof(argv[2]); // if argv[2] is not a float it will be set to 0
        if(p > 1 || p < 0){
            fprintf(stderr, "P is a probability. It has to be a value 0 <= x <= 1.\n");
        	exit(EXIT_FAILURE);
        }

        int t = atoi(argv[3]); // if argv[3] is not an int it will be set to 0

        printf("Number of processes: %d\nProbability of delay: %f\nTime of delay: %d\n", n, p, t);
        

        /* creating all the pipes */
        for(int i = 1; i <= n; i++){
	        if(i != n)
                buildPipename(pipename, i, i + 1);
			else
			    buildPipename(pipename, n, 1);
            printf("%s\n", pipename);
            mkfifo(pipename, 0777);
        }
   
        
        /* creating all the processes */
        for(int i = 1; i <= n && child == 0; i++){
    		if((pid = fork()) == -1) {
			    fprintf(stderr, "%s: can’t fork command: %s\n", argv[0], strerror(errno));
			    continue;
		    }
		    else if(pid == 0) //child
		    {
                process_id = i;
                //printf("Process %d with i: %d \n", getpid(), i);
                child = 1;
            }
        }

        srand(time(0) * getpid()); //setting the seed for the random number generation. Using the pid to guarantee different seeds for different processes

        /* opening the read and write files for the processes */
        if(process_id == 1){
            buildPipename(pipename, n, 1); // process 1 reads from the last pipe, pipento1
            readPipe = open(pipename, O_RDONLY); 
        }
        if(process_id == n){
            buildPipename(pipename, n, 1); // process n writes to pipento1
            writePipe = open(pipename, O_WRONLY); 
        }
        if(process_id != 1){
            buildPipename(pipename, process_id-1, process_id); // process x reads from the last process that wrote
            readPipe = open(pipename, O_RDONLY); 
        }
        if(process_id != n){
            buildPipename(pipename, process_id, process_id+1); // process x writes for the next process
            writePipe = open(pipename, O_WRONLY); 
            if(process_id == 1){ //writing the token on the first pipe
                write(writePipe, &token, sizeof(int));
            }
        }
        
        while(process_id != -1){
            read(readPipe, &token, sizeof(int));
            xr = rand() / ((float) RAND_MAX);
            token++;
            //printf("token: %d process id:%d random:%.2f\n", token, process_id, xr);

            if(xr <= p){
                printf("[p%d] locked on token (val = %d)\n", process_id, token);
                sleep(t);
                printf("[p%d] unlock token\n", process_id);
            }

            write(writePipe, &token, sizeof(int));
        }
          

        if(child != 0){
            close(readPipe);
            close(writePipe);
            unlink(pipename);
            exit(0);
        }

        while(process_id==-1){
	        for(int j=0; j<n; j++)
            	wait(NULL);
        }
		
	return EXIT_SUCCESS;
}