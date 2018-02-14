#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
struct my_msgbuf 
{ 
	long mtype;
	char mtext[200];
};
int main(void) 
{
	struct my_msgbuf buf,rbuf; 
	int msqid,rmsqid;
	key_t key,rkey;
	if ((key = ftok("kirk.c", 'B')) == -1) //sender key
	{
		perror("ftok");
		exit(1);
	 }
	if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) //sender buffer
	{
		perror("msgget");
		exit(1);
	}
	if ((rkey = ftok("kirk2.c", 'B')) == -1)//receiver key 
	{
		perror("ftok");
		exit(1);
	 }
	if ((rmsqid = msgget(rkey, 0654 | IPC_CREAT)) == -1) //receiver buffer
	{
		perror("msgget");
		exit(1);
	}
	printf("Enter lines of text, ^D to quit:\n");
	buf.mtype = 1; 
	
	while(1)
	{
		fgets(buf.mtext, sizeof buf.mtext, stdin);
		int len = strlen(buf.mtext);
		/* ditch newline at end, if it exists */
		if (buf.mtext[len-1] == '\n')
			 buf.mtext[len-1] = '\0';
		if (msgsnd(msqid, &buf, len+1, 0) == -1) //send the math equation 		
			perror("msgsnd");
        if (msgrcv(rmsqid, &rbuf, sizeof(rbuf.mtext), 0, 0) == -1) //receive the result
        {
            perror("msgrcv");
            exit(1);
        }
		printf("server: \"%s\"\n", rbuf.mtext);
    	
		
	}
	
		return 0; 
}