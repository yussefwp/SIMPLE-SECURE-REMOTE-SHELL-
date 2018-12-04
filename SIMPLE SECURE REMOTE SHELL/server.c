#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>     
#include <time.h> 
#include <arpa/inet.h>
#include <sys/wait.h>

#include "util.h"


// execute the client commands
void exe_client(int sd);

// execute the command
PList* exe_command(PList* jobs, HistoryBuf* h, char* cmd, int sd, int* quit);

// parse arguments return number of arguments
int parse_arg(char* args[MAX_CMDS], char* line);

int main()
{
    int sd;
    struct sockaddr_in addr;
    struct sockaddr_in caddr;
    int csd;
    int addrlen;
    pid_t pid;
    
    // create the socket    
    if ( ( sd = socket( AF_INET, SOCK_STREAM, 0) ) == -1 )
    {
        perror( "Error socket\n");
        exit( 1 );
    }

    memset( &addr, 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( SVR_PORT );  
    addr.sin_addr.s_addr = INADDR_ANY;    
    

    // bind the server socket
    if ( bind( sd, ( struct sockaddr *) &addr, sizeof( addr ) ) == -1)
    {
        printf( "Error bind\n" );
        exit( 1 );
    }  

    if ( listen( sd, 5 ) == -1 ) 
    {
        printf( "Error listen\n" );
        exit( 1 );
    }
    
    while (1)
    {     
        // a new client connected      
        addrlen = sizeof(caddr);
        csd = accept(sd, (struct sockaddr *) &caddr, &addrlen);
    
        if (csd != -1)
        {
            printf("new client enter\n");
            pid = fork();
            if (pid < 0)
            {
                printf("Error fork\n");
                exit(1);
            }
            if (pid == 0)
            {
                // child process
                close(sd);
                exe_client(csd);
                return 0;
            }
            // parent continue 
            close(csd);
        }
    }

    return 0;       
}
   
// execute the client commands
void exe_client(int sd)
{
    char line[MAX_LINE];
    char* p;
    char* pnext;
    int quit = 0;
    
    PList* jobs = NULL;
    HistoryBuf* history = create_history();
    
    while (quit == 0)
    {
        // read the command from the client
        if (decrypt_read_line(sd, line) < 0)
            break;
            
        jobs = reap_job(jobs, 0);
        
        p = line;
        do
        {
            pnext = strchr(p, ';');
            if (pnext != NULL)
            {
                pnext[0] = 0;
                pnext++;
			}
			
			jobs = exe_command(jobs, history, p, sd, &quit);               
            
            p = pnext;
		} while (p != 0);           
        
        // wait for 300 milliseconds
        msleep(300);         
    
        // write ending tag
        write_line(sd, "");
        write_line(sd, END_TAG);
    }
    printf("client quit\n");
    free(history);
    close(sd);
    reap_job(jobs, 1);
}
  
// parse arguments return number of arguments
int parse_arg(char* args[MAX_CMDS], char* line)
{  
    int numArgs = 0;
        
    args[numArgs] = strtok(line, " \t\r\n");
    while (args[numArgs] != NULL) {
        numArgs++;          
        args[numArgs] = strtok(NULL, " \t\r\n");            
    }    
    return numArgs;        
}

// execute the command
PList* exe_command(PList* jobs, HistoryBuf* h, char* cmd, int sd, int* quit)
{
    char buff[MAX_LINE];
	char line[MAX_LINE];   
    char* args[MAX_CMDS];
    int numArgs = 0;
    int no;
    pid_t pid;
    
    strcpy(buff, cmd);
    
    numArgs = parse_arg(args, cmd);
    
    if (numArgs == 0)
    {
        quit[0] = 1;
        return jobs;
	}
            
    if (strcmp(args[0], "quit") == 0)
    {
        return jobs;
	} 	
	
	if (strcmp(args[0], "history") == 0 || strcmp(args[0], "History") == 0)
	{
	    list_history(h, sd);
	    return jobs;
	}
	
	if (strcmp(args[0], "!!") == 0)
	{  
         if (get_history(h, line, h->size) < 0)
         {
             write_line(sd, "No commands in history");
             return jobs;
         }
         strcpy(buff, line);   
         numArgs = parse_arg(args, line);
	} 
	else if (args[0][0] == '!' && isdigit(args[0][1]))
	{
        no = atoi(args[0]+1);
        if (get_history(h, line, no) < 0)
        {    
            write_line(sd, "No such command in history");
            return jobs;
        }
        strcpy(buff, line);   
        numArgs = parse_arg(args, line);     
    }
	   
    if (strcmp(args[0], "jobs") == 0)
    {
        list_jobs(jobs, sd);
        add_history(h, buff);
        return jobs;
	}
	         
    pid = fork();
    if (pid == -1)
    {
        quit[0] = 1;
        return jobs;
	}
        
    if (pid == 0)
    {
        // process which execute the command 
        close(STDOUT_FILENO);  
        close(STDERR_FILENO);                                         
        dup2(sd, STDOUT_FILENO);                                      
        dup2(sd, STDERR_FILENO);
        close(sd);
        execvp(args[0], args); 
        exit(1);
    }
    else
    {                      
        add_history(h, buff);
        jobs = add_job(jobs, pid);
	}
    return jobs;
}

