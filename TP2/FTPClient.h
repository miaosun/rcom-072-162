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
#include <string.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

#define MAX_MSG_LEN 255
#define MAX_WORD_LEN 100

#define PRINT_BLUE(format,args...) printf("\033[34m"format"\033[0m", ##args);
#define PRINT_GREEN(format,args...) printf("\033[32m"format"\033[0m", ##args);
#define PRINT_ERROR(format, args...) printf("\033[31m"format"\033[0m", ##args);

int parse_addr(char * buffer);
int recebe(int sock_fd);
int ligar(char * hostname, int port);
int disconnect(int socket_fd);

int con_pasv(void);
void retr(void);
int recebe_ficheiro(int sock_fd);

int exec(void);
