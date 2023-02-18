#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char* argv[]) 
{	
	pid_t pid;
	if (argc < 2) 
    {
        fprintf(stderr, "No txt files were given\n");
        exit(EXIT_FAILURE);
    }
    
 
	for(int i=1;i<argc;i++)
	{	
		int fd = open(argv[1], O_RDONLY);
		if (fd == -1) {
			printf("error: cannot open %s\n", argv[1]);
			return EXIT_FAILURE;
		}
		
		
		
		if((pid = fork()) == -1) {
			fprintf(stderr, "%s: can’t fork command: %s\n",argv[0],strerror(errno));
			continue;
		}
		
		else if(pid == 0) {
			
			printf("[%d] converting %s \n", getpid(),argv[i]);
			char aux[80]="";
			strcpy(aux, argv[i]);
;			int size = strlen(aux);
			aux[size-4] = '\0';
			strcat(aux,".epub");
			
			execlp("pandoc","pandoc",argv[i],"-o",aux,NULL);
		
			break;
		}
		else{
			wait(NULL);
			
		}
	}
	
	execlp("zip","zip","-R","ebooks","*.epub",NULL);
	return EXIT_SUCCESS;
}
