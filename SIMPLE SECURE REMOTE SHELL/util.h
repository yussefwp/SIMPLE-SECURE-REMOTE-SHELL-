

#define MAX_LINE  1024
#define END_TAG   "[SVR-ENDING]"
#define SVR_PORT  12345

#define MAX_CMDS  128 
 
#define MAX_HISTORY   10  

typedef struct _plist
{
    struct _plist* next;
    pid_t pid;

} PList;

typedef struct _historybuf
{
    char buf[MAX_HISTORY][MAX_LINE];  // commands buf
    
    int start;     // start position
    int size;      // number of commands
    
} HistoryBuf; 

// read a line into buf, return -1 if failed
int read_line(int fd, char *buf); 
     
// write a line into buf, append '\n', return -1 if failed
int write_line(int fd, char *buf);


// read and decrypt a line into buf, return -1 if failed
int decrypt_read_line(int fd, char *buf); 
     
// encrypt and write a line into buf, append '\n', return -1 if failed
int encrypt_write_line(int fd, char *buf);

// sleep for milliseconds
void msleep(int mill);

// append pid to list
PList* add_job(PList* old, pid_t pid);

// reap Zombie Processes, if waiting is 1, the process will wait util all quit
PList* reap_job(PList* old, int waiting);

// write list of jobs to the client
void list_jobs(PList* old, int fd);

// create history buff
HistoryBuf* create_history();

// add command to history 
void add_history(HistoryBuf* h, char* cmd);

// write list of commands history to the client
void list_history(HistoryBuf* h, int fd);

// get command by number, store the command to output, return -1 if failed
int get_history(HistoryBuf* h, char* output, int num);