#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include<ctype.h>
#include<math.h>


long calculate(char* exp,int *breakpt)//math calculation
{
    long res =0;
    long num[100];
    int i=0,j=0;
    char  *p = exp;
    long val;
    while (*p) 
    { 
        if (isdigit(*p)) 
        { 
            val = strtol(p, &p, 10); 
            num[i]=val;
            i++;

        }
        p++;
    }
    p = exp;
    i=0;
    res = num[0];
    while (*p)
    {
        if(!isdigit(*p))
        { 
            if (*p =='+')
            {
                res=res+num[i+1];
            }
            else if (*p =='*')
            {
                res = res*num[i+1];
           
            }
            else if (*p == '-')
            {
                res=res-num[i+1];
            }
            
            else if (*p=='^')
            {
                res=pow(res,num[i+1]);
            }
            else if (*p=='%')
            {
                res= res%num[i+1];
            }

            else
            {
                *breakpt =1;
                break;
            }
            
            i++;
        }
        p++;
    }
   
    return res;
}

struct my_msgbuf 
    {
    long mtype;
    char mtext[200];
    };

int main(void)
{
    struct my_msgbuf buf,sbuf;
    int msqid,smsqid;
    key_t key,skey;

    if ((key = ftok("kirk.c", 'B')) == -1) {  /* same key as kirk.c */
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644)) == -1) { /* connect to the receiver queue */
        perror("msgget");
        exit(1);
    }
    if ((skey = ftok("kirk2.c", 'B')) == -1) {  /* same key as kirk2.c */
        perror("ftok");
        exit(1);
    }

    if ((smsqid = msgget(skey, 0654)) == -1) { /* connect to the sender queue */
        perror("msgget");
        exit(1);
    }
    printf("hey user! I am ready to compute.\n");
    sbuf.mtype =1;
    for(;;)
    { /* Spock never quits! */
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) //receive the math equation
        {
            perror("msgrcv");
            exit(1);
        }
        printf("spock: \"%s\"\n", buf.mtext);
        char * exp = buf.mtext;
        int breakpt =0;
        long result = calculate(exp,&breakpt);
        if(breakpt ==0)
        {
            sprintf(sbuf.mtext, "%ld", result );
            int len = strlen(sbuf.mtext);
            if (sbuf.mtext[len-1] == '\n')sbuf.mtext[len-1] = '\0';
            if (msgsnd(smsqid, &sbuf, len+1, 0) == -1)perror("msgsnd");//send the result
        }
        else
        {
            strcpy(sbuf.mtext,"unknown operator");
            int len = strlen(sbuf.mtext);
            if (sbuf.mtext[len-1] == '\n')sbuf.mtext[len-1] = '\0';
            if (msgsnd(smsqid, &sbuf, len+1, 0) == -1)perror("msgsnd");//send the result
        }
    }

    return 0;
}