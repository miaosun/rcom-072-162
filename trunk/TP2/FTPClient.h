#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>



#define MAX_CMD_LEN 255
#define MAX_WORD_LEN 20

int askCmd(char * buffer);
int parseCmd(char * buffer);
void clear_last_cmd(char** com);
int exec_cmd(char ** com);
int ligar(char * hostname, int port);
int authenticate(void);
int getFile(char * filename);
int disconnect(int socket_fd);
