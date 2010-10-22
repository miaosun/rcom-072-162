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


volatile int STOP=FALSE;
int passou=FALSE, mode;
char buf[255];

void atende()                   // atende alarme
{
	passou=TRUE;
}

void alarme(int n)
{

	(void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

	alarm(n);                 // activa alarme de 3s
}

int recebe(int fd)
{
	char aux;
	int res=-1;
	int itt=0;	
	//leitura da trama para buf
	while (res!=1)
	{
    	res = read(fd,&aux,1); 
		if(passou) return FALSE;
	}
	res=-1;
	if(aux==FLAG)
	{
		buf[itt]=aux;
		itt++;
		aux=0x00;
		if(passou) return FALSE;
		while(aux!=FLAG)
		{
			while (res!=1)
			{
				res = read(fd,&aux,1); 
				if(passou) return FALSE;
			}
			if(passou) return FALSE;
			res=-1;
			buf[itt]=aux;
			itt++;
		}
		if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_UA && buf[3]==(A_Snd_to_Rcv^C_UA) && buf[4]==FLAG)
		{
			printf("recebi trama UA!\n");
			return TRUE;
		}
		printf("recebi trama errada!\n");
		return FALSE;
	}
}

int fecha(int fd)
{
	char aux;
	int res=-1;
	int itt=0;	
	//leitura da trama para buf
	while (res!=1)
	{
    	res = read(fd,&aux,1); 
		if(passou) return FALSE;
	}
	res=-1;
	if(aux==FLAG)
	{
		buf[itt]=aux;
		itt++;
		aux=0x00;
		if(passou) return FALSE;
		while(aux!=FLAG)
		{
			while (res!=1)
			{
				res = read(fd,&aux,1); 
				if(passou) return FALSE;
			}
			if(passou) return FALSE;
			res=-1;
			buf[itt]=aux;
			itt++;
		}
		if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_DISC && buf[3]==(A_Snd_to_Rcv^C_DISC) && buf[4]==FLAG)
		{
			printf("recebi trama DISC!\n");
			return TRUE;
		}
		printf("recebi trama errada!\n");
		return FALSE;
	}

}

int main(int argc, char** argv)
{
    int fd[2],c, res;
    struct termios oldtio,newtio;
    
    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) 
	{
    	/*printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    	exit(1);*/
		if((fd[0]=open("R_2_E", FIFO_READ))==-1){
			printf("failed to open fifo: R_2_E\n");
			return 1;
  		}
		printf("abriu fifo de entrada\n");
		if(mkfifo("E_2_R", FIFO_PERMS)!=0)
		{
			printf("failed to create fifo E_2_R\n");
			exit(1);
		}
		printf("criou fifo de saida\n");
		if((fd[1]=open("E_2_R", FIFO_WRITE))==-1){
			printf("failed to open fifo: E_2_R\n");
			return 1;
  		}
		printf("abriu fifo de saida\n");
		mode=PIPE;
    }
	else mode=SERIAL;



	if(mode==SERIAL)
	{
	  /*
		Open serial port device for reading and writing and not as controlling tty
		because we don't want to get killed if linenoise sends CTRL-C.
	  */


		fd[0] = open(argv[1], O_RDWR | O_NOCTTY );
		fd[1] = fd[0];
		if (fd[0] <0) {perror(argv[1]); exit(-1); }

		if ( tcgetattr(fd[0],&oldtio) == -1) { /* save current port settings */
		  perror("tcgetattr");
		  exit(-1);
		}

		bzero(&newtio, sizeof(newtio));
		newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
		newtio.c_iflag = IGNPAR;
		newtio.c_oflag = 0;

		/* set input mode (non-canonical, no echo,...) */
		newtio.c_lflag = 0;

		newtio.c_cc[VTIME]    = 10;   /* inter-character timer unused */
		newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



	  /* 
		VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
		leitura do(s) próximo(s) caracter(es)
	  */



		tcflush(fd[0], TCIOFLUSH);

		if ( tcsetattr(fd[0],TCSANOW,&newtio) == -1) {
		  perror("tcsetattr");
		  exit(-1);
		}

		printf("New termios structure set\n");
	}

	/*************para mandar uma cadeia de caracteres***********
    gets(buf); 
	
	int itt;
	itt=strlen(buf);

	res = write(fd,buf,itt+1);   
    printf("%d bytes written\n", res);
	***********************************************************/


	/******************* para mandar trama SET *****************
	******** criar a trama FLAG A_Snd_to_Rcv C BCC FLAG
	******** enviar trama em vez do texto*/
	buf[0]=FLAG;
	buf[1]=A_Snd_to_Rcv;
	buf[2]=C_SET;
	buf[3]=(A_Snd_to_Rcv^C_SET);
	buf[4]=FLAG;



	res = write(fd[1],buf,5);   
	printf("enviei trama SET! com %d bytes\n", res);
	alarme(3);
	/**************** para receber trama UA ****************/	
	while(!recebe(fd[0]))
	{
		passou=FALSE;
		alarme(3);
		res = write(fd[1],buf,5);   
		printf("enviei trama SET! com %d bytes\n", res);
	}	


	/************** llclose ****************/
	buf[0]=FLAG;
	buf[1]=A_Snd_to_Rcv;
	buf[2]=C_DISC;
	buf[3]=(A_Snd_to_Rcv^C_DISC);
	buf[4]=FLAG;
	
	res = write(fd[1],buf,5);   
	printf("enviei trama DISC! com %d bytes\n", res);
	alarme(3);
	while(!fecha(fd[0]))
	{
		passou=FALSE;
		alarme(3);
		res = write(fd[1],buf,5);   
		printf("enviei trama DISC! com %d bytes\n", res);
	}

	buf[0]=FLAG;
	buf[1]=A_Rcv_to_Snd;
	buf[2]=C_UA;
	buf[3]=(A_Rcv_to_Snd^C_UA);
	buf[4]=FLAG;

	res = write(fd[1],buf,5);   
	printf("enviei trama UA! com %d bytes\n", res);


/************* para receber uma cadeia de caracteres ****************
	itt=0;
	char aux;
	res=-1;
	//espera para receber trama UA durante 3s e processa trama, senao volta a enviar
    while (1) 
	{
		while (res!=1)
        	res = read(fd,&aux,1);
		res=-1;
		buf[itt]=aux;
		if(aux=='\0') break;
		itt++;
    }

	//buf[itt+1]='\0'; 
	printf("%s\n", buf);
***************************************************************/

  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
  */

	sleep(2);
	if(mode==SERIAL)
		tcsetattr(fd,TCSANOW,&oldtio);
	else 
		close(fd[1]);

	close(fd[0]);

    return 0;
}
