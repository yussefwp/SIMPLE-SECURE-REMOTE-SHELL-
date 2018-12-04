# SIMPLE-SECURE-REMOTE-SHELL-
A.Encryption/Decryption of Command: Client Encrypts the Command entered by the user  before  sending  it  to  the  Server.    Server,  upon  receiving  the  encrypted  command, decrypts it before executing the command.You  are  required  to  write  the  Encryption  and  Decryption  functionsfor  the following simple algorithm:Encryption: Add  a  number“N”to  the  ASCII values  of  all  the  characters  of  the commandto get the Encrypted commandDecryption:Subtract the number “N” from the ASCII values of all the characters of the commandto get the original commandWhere,N......is the last digitof yourISU ID (mention it in the code using comments)(If the last digit of your ISU ID is 0, use the number before that)B.“jobs” command:You are to implement thejobs command, so that when thecommand “jobs” is enteredby user, it displays the list of the process IDs (PIDs)of all “live”child processes(i.e. the processesthat have not terminatedyet)C.Reaping of Zombie Processes:Every time, when input is received at server, you should check for any Zombie processes, and if found, reap them. You can use the list of child processes PIDsto check for zombie processes.
D.“History”Command:a)You are to implement a history feature in your program. When the user enters the command “History”, it displays up*to 10 most recent commands entered by the user. The commands are numbered from 1 to 10, 1 being the oldest and 10 being the most recent command. *  if the user has so far entered less than 10 commands, say 6, display all the 6 commands). Next  your  program  should  support  two  techniques for  retrieving  commands from the command history:b)When  the  user  enters !!as  a  command,  the  most  recent command  in  the  history(i.e.  the command just before entering !!)is executed.c)When the user enters !Nas a command, the Nth command in the history is executed, (a single ! followed by an integer N, e.g. !3 means 3rdcommand in history)(where N = 1,2,3,......,10)d)Theprogram should also manage basic error handling.1.If there are no commands in the history, entering !! should result in a message “No commands in history”. 2.If there is no command corresponding to the number entered with!N, the program should output "No such command in history."E.Handling MultiplecommandsIn actual Linux shell multiple commands can be entered in the same line separated by ";".In Your Program:The user (on client side) should be able to enter multiple commands in the same lineseparated by ; , the client sends all the commands to the server, receives output for these commands from the server, and displays the outputs on its screen.Example, if the user wants to enter 3 commands in the same line,-bash-4.3$ date;cal;who with the following output (outputs of the three commands):Wed Sep 13 12:55:51 CDT 2017September 2017Su Mo Tu We Th Fr Sa1  23  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30abi     pts/0      2018-01-28 13:24 (10.64.222.89)abi      pts/1        2018-01-28 13:31 (10.64.222.89)mabdulah pts/2        2018-01-28 14:39 (10.25.71.69)jcupts/8        2018-01-19 17:14 (10.24.46.63)
NOTE:The basic  working  of  the  project  remains  the  same with “jobs”, “History”, and “multiple commands” features. Meaning that all commands are entered on client side,  client  sends  the  commands  (including  jobs,  history,  and  the  multiple  commands entered  by  the  user)  to  the  server,  the  server  executes  the  commands  and  sends  the outputs back to client, and then the client displays the output on its screen.You  will  write the  Server  and Clientprogram(as  this  project  is  extension  of  project  1, some steps are the same):Server:The server will run on the remote machine. ▪It  will  bind  to  a TCPsocket  at  a  port  known  to  the  client  and  waits  for  a Connection Request from Client.▪When  it  receives  a  connection,itforksa  child  process  to  handle  this connection.The Server must handle multiple clients at a time.▪The parent process loops back to wait for more connections.▪The command received is Decrypted.▪The child  processexecutes  the  given  shell  command(received  from  the client), returning allstdoutandstderrto the client.(Hence, the server will notdisplay the output of the executed command)▪The server can assume that the shell command does not usestdin.Client:The clientwill run on the local machine.▪From  the  command  line,  the  user  will  specify  the  host(where  the  server resides)and the commandto be executed.▪The client will then connect to the server via a TCP socket.▪The Client Encryptsthe Command entered by the user.▪The Client sendsthe commandto the server.▪The client will display any output received from the server to thestdout.▪After displaying the output, the client waits for next command from the user. ▪The client will notclose/exit until the user enters “quit” command.