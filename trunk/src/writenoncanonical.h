/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define MAX_BYTES 25 //maximo tamanho da trama
#define MAX_REPEAT 3 //para max num de retransmissoes
#define TIMEOUT 3 //segundos para esperar antes de retransmitir

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
#define RR0 0x01
#define RR1 0x21
#define REJ0 0x05
#define REJ1 0x25
#define N0 0x00
#define N1 0x02

void atende();                   // atende alarme
int recebe_UA(int fd);
int recebe_DISC(int fd);
int recebe_RR(int fd);

int llopen(int fd[2]);
int llwrite(int fd[2], char * buffer, int length);
int llclose(int fd[2]);

int l_read(int fd);//le uma trama para buf, retorna o tamanho da trama ou -1 se erro


