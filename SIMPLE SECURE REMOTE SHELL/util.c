#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>     
#include <time.h>  
#include <sys/wait.h>
					  
#include "util.h"

#define UID  3

                      
// read a line into buf, return -1 if failed
int read_line(int fd, char *buf)
{
    int sum = 0, r;
    do
    { 
        r = read(fd, buf + sum, 1);
        if (r <= 0)
            return -1;
        sum ++;
    } while (buf[sum-1] != '\n');
    buf[sum-1] = 0;
    return 0;
} 
     
// write a line into buf, append '\n', return -1 if failed
int write_line(int fd, char *buf)
{  
    int len = strlen(buf);
    int sum = 0, r;
    char tmp = '\n';
    
    while (sum < len)
    {
        r = write(fd, buf + sum, len - sum);
        if (r < 0)
            return -1;
        sum += r;
    }
    
    if (write(fd, &tmp, 1) < 0)
        return -1;
    return 0;    
}
   
// read and decrypt a line into buf, return -1 if failed
int decrypt_read_line(int fd, char *buf)
{   
    int sum = 0, r;
    do
    { 
        r = read(fd, buf + sum, 1);
        if (r <= 0)
            return -1;
        buf[sum] -= UID;    
        sum ++;
    } while (buf[sum-1] != '\n');
    buf[sum-1] = 0;
    return 0;
} 
     
// encrypt and write a line into buf, append '\n', return -1 if failed
int encrypt_write_line(int fd, char *buf)
{   
    int len = strlen(buf);
    int sum = 0, r, i;
    char tmp = (char)('\n' + UID);
    for (i = 0; i < len; i++)
        buf[i] += UID;
    
    while (sum < len)
    {
        r = write(fd, buf + sum, len - sum);
        if (r < 0)
            return -1;
        sum += r;
    }
    
    if (write(fd, &tmp, 1) < 0)
        return -1;
    return 0;    
}
  
// sleep for milliseconds
void msleep(int mill)
{  
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000 * mill;
    nanosleep(&ts, NULL);
}
  
// append pid to list
PList* add_job(PList* old, pid_t pid)
{
    PList* lst = old; 
    
    PList* tmp = (PList*)malloc(sizeof(PList));
    tmp->pid = pid;
    tmp->next = NULL;
    
    if (old == NULL)
        lst = tmp;
    else
    {
        while (old->next != NULL)
            old = old->next;
        old->next = tmp;
	}
        
    return lst;    
}

// reap Zombie Processes, if waiting is 1, the process will wait util all quit
PList* reap_job(PList* old, int waiting)
{
    PList* lst = old;
    if (waiting == 1)
    {
        while (lst != NULL && waitpid(lst->pid, NULL, 0) >= 0)
        {  
	        old = lst;
	        lst = lst->next;
	        free(old);
		}
	}
	else
	{
	    while (lst != NULL && waitpid(lst->pid, NULL, WNOHANG) > 0)
	    {
	        old = lst;
	        lst = lst->next;
	        free(old);
		}
	}
	return lst;
}

// write list of jobs to the client
void list_jobs(PList* old, int fd)
{
    char buff[128];
    if (old == NULL)
    {
        write_line(fd, "No jobs");
	}
	else
	{
        while (old != NULL)
        {
            sprintf(buff, "%d", old->pid);  
            write_line(fd, buff);
            old = old->next;
	    }
	}
}
  
// create history buff
HistoryBuf* create_history()
{
    HistoryBuf* buf = (HistoryBuf*)malloc(sizeof(HistoryBuf));
    buf->start = 0;
    buf->size = 0;
    return buf;
} 

// add command to history 
void add_history(HistoryBuf* h, char* cmd)
{   
    int idx = (h->start + h->size) % MAX_HISTORY;      

    strcpy(h->buf[idx], cmd);
   
    if (h->size == MAX_HISTORY) 
    {
        // full
        h->start = (h->start + 1) % MAX_HISTORY;
    }
    else
    {
        h->size++;
    }
}

// write list of commands history to the client
void list_history(HistoryBuf* h, int fd)
{
    char buff[128];
    for (int i = 0; i < h->size; i++)
    {
        sprintf(buff, "%d %s", i+1, h->buf[i]);  
        write_line(fd, buff);         
	}
}

// get command by number, store the command to output, return -1 if failed
int get_history(HistoryBuf* h, char* output, int num)
{  
    int idx;
    if (h->size == 0 || num < 1 || num > h->size)
        return -1;
      
    idx = (h->start + num - 1) % MAX_HISTORY;
    strcpy(output, h->buf[idx]);
      
    return 0;
}

