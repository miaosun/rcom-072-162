/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

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

volatile int STOP=FALSE;
int mode;

int main(int argc, char** argv)
{
    int fd[2],c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) 
	{
    	/*printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    	exit(1);*/
		if(mkfifo("R_2_E", FIFO_PERMS)!=0)
		{
			printf("failed to create fifo R_2_E\n");
			exit(1);
		}
		printf("criou fifo de saida\n");
		if((fd[1]=open("R_2_E", FIFO_WRITE))==-1){
			printf("failed to open fifo: R_2_E\n");
			return 1;
  		}
		sleep(1);
		printf("abriu fifo de saida\n");
		if((fd[0]=open("E_2_R", FIFO_READ))==-1){
			printf("failed to open fifo: E_2_R\n");
			return 1;
  		}
		printf("abriu fifo de entrada\n");
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

		newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
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

/********** para receber uma cadeia de caracteres **************
	int itt;
	itt=0;
	char aux;
	res=-1;
    while (1) 
	{       
		while (res!=1)
        	res = read(fd,&aux,1); 
		res=-1;
		buf[itt]=aux;				
		if (aux=='\0')	break;
		itt++;		
    }

	//buf[itt+1]='\0';               so we can printf...
	printf("%s\n", buf);

	
	write(fd,buf,itt+1);
******************************************************************/

/****************** para receber trama SET e trata-la ****************/	
	//vamos usar buf
	char aux;
	res=-1;
	int itt=0;	
	int acabou = FALSE;
	//leitura da trama para buf
	while (acabou == FALSE) 
	{
		res=-1;
		itt=0;
		while (res!=1)
        		res = read(fd[0],&aux,1); 
		res=-1;
		if(aux==FLAG)
		{
			buf[itt]=aux;
			itt++;
			aux=0x00;
			while(aux!=FLAG)
			{
				while (res!=1)
					res = read(fd[0],&aux,1); 
				res=-1;
				buf[itt]=aux;
				itt++;
			}
			if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_SET && buf[3]==(A_Snd_to_Rcv^C_SET) && buf[4]==FLAG)
			{
				printf("recebi trama SET!\n");
				buf[0]=FLAG;
				buf[1]=A_Snd_to_Rcv;
				buf[2]=C_UA;
				buf[3]=(A_Snd_to_Rcv^C_UA);
				buf[4]=FLAG;
				res=write(fd[1],buf,5);
				printf("enviei trama UA! com %d bytes\n", res);
				acabou = TRUE;
			}
		}
	}
/********FIM********** para receber trama SET e trata-la *******FIM*********/	

/****************** para receber trama DISC e trata-la ****************/	

	while(1){
		
		res=-1;
		itt=0;
		while (res!=1)
        		res = read(fd[0],&aux,1); //vai lendo caracteres
		res=-1;

		if(aux == FLAG){ //se encontrar a FLAG enquanto le os caracteres, entao vai tratar a trama
			buf[itt]=aux; //criando a trama, guarda na primeira posicao do buffer a FLAG inical da trama
			itt++;
			aux=0x00; //desactiva a FLAG para poder ir reconhecer o resto da trama
			while(aux!=FLAG) //enquanto nao encontrar outra FLAG (que indica que terminou a trama),  
			{		//vai processala
				while (res!=1)
					res = read(fd[0],&aux,1); 
				res=-1;
				buf[itt]=aux;
				itt++;
			}
			if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_DISC && buf[3]==(A_Snd_to_Rcv^C_DISC) && buf[4]==FLAG) //se no buffer ja esta a trama toda comecando e acabando na FLAG, entao informa que recebeu e reenvia
			{
				printf("recebi trama DISC!\n");
				res=write(fd[1],buf,5);
				printf("enviei trama DISC! com %d bytes\n", res);
				break;
			}
		}

	}

/********FIM********** para receber trama DISC e trata-la *******FIM*********/

/****************** para receber trama UA final e trata-la ****************/

	while (1) 
	{
		res=-1;
		itt=0;
		while (res!=1)
        		res = read(fd[0],&aux,1); 
		res=-1;
		if(aux==FLAG)
		{
			buf[itt]=aux;
			itt++;
			aux=0x00;
			while(itt<5)//aux!=FLAG)
			{
				while (res!=1)
					res = read(fd[0],&aux,1); 
				res=-1;
				buf[itt]=aux;
				itt++;
			}
			if(buf[0]==FLAG && buf[1]==A_Rcv_to_Snd && buf[2]==C_UA && buf[3]==(A_Rcv_to_Snd^C_UA) && buf[4]==FLAG)
			{
				printf("recebi trama UA!\n");
				printf("Conclui com exito a transmissao de pacotes\n");
				break;
			}
			printf("recebi trama errada\n");
		}
	}

/********FIM********** para receber trama UA final e trata-la *******FIM*********/	

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */

	sleep(2);
	if(mode==SERIAL)
		tcsetattr(fd[0],TCSANOW,&oldtio);
	else 
		close(fd[1]);

	close(fd[0]);

    return 0;
}
