#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
int main(void)
{
	int fd[2], nbytes;
	pid_t childpid;
	char string[1000];
	char readbuffer[1000];
	pipe(fd); 
	childpid = fork();
	if(childpid == -1) 
	{
		perror("fork");
		exit(1);
	}
	if(childpid == 0)
	{
		/* Child process closes up input side of pipe */ 
		close(fd[0]);
		FILE *fp;
 		char path[100];

 		 /* Open the command for reading. */
  		fp = popen("/bin/ls -l *.jpg", "r");
  		if (fp == NULL) 
  		{
    		printf("Failed to run command\n" );
   			 exit(1);
 		 }

  		/* Read the output a line at a time - output it. */
  		while (fgets(path, sizeof(path), fp) != NULL) 
  		{
  			strcat (string,path);
  		}
  		printf("Child process:\n Sending data:\n %s\n",string );
  		write(fd[1], string, (strlen(string)+1));
  		/* close */
  		pclose(fp);
		exit(0);
	} 
	else 
	{
		/* Parent process closes up output side of pipe */ 
		close(fd[1]);
		char *path;
		/* Read in a string from the pipe */
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer)); 
		FILE * fp;
		char command[1000];
		strcpy(command,"echo ' ");
		strcat(command,readbuffer);
		strcat(command, " ' > input.txt ");
		fp =popen(command,"r");
		if (fp == NULL) 
  		{
    		printf("Failed to run command\n" );
   			 exit(1);
 		}
 		pclose(fp);
 		printf("Parent process:\n");
 		system("wc -l input.txt");
	}
	return(0); 
}