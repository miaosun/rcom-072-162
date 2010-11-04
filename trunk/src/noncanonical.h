/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define MAX_BYTES 25 //maximo tamanho da trama

#define SERIAL 1
#define PIPE 0

#define FIFO_PERMS (S_IRWXU | S_IWGRP | S_IWOTH) 
#define FIFO_READ O_RDONLY// | O_NONBLOCK
#define FIFO_WRITE O_WRONLY

#define FLAG 0x7E
#define A_Rcv_to_Snd 0x01
#define A_Snd_to_Rcv 0x03 
#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0B
#define RR0 0x01
#define RR1 0x21
#define REJ0 0x05
#define REJ1 0x25
#define N0 0x00
#define N1 0x02

int l_read(int fd[2]);//le uma trama para buf, retorna o tamanho da trama ou -1 se erro

int llopen(int fd[2]);
int llread(int fd[2], char * buffer);
int llclose(int fd[2]);

void envia_UA(int fd[2]);
void envia_RR(int fd[2]);
void envia_REJ(int fd[2]);

int start(int fd[2]);
int receive(int fd[2], int filedes);
