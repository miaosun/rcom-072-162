/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FIFO_PERMS (S_IRWXU | S_IWGRP | S_IWOTH) 
#define FIFO_READ O_RDONLY// | O_NONBLOCK
#define FIFO_WRITE O_WRONLY

#define SERIAL 1
#define PIPE 0

//trama
#define FLAG 0x7E
#define A_Rcv_to_Snd 0x01
#define A_Snd_to_Rcv 0x03 
#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0B

void atende();                   // atende alarme
void alarme(int n);
int recebe_UA(int fd);
int recebe_DISC(int fd);
void envia_SET(int fd[2]);
void envia_DISC(int fd[2]);

