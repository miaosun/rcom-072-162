/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>


#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

//trama set
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07
#define BCC 0x00

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

	/*************para mandar uma cadeia de caracteres***********
    gets(buf); 
	
	int itt;
	itt=strlen(buf);

	res = write(fd,buf,itt+1);   
    printf("%d bytes written\n", res);
	***********************************************************/


	/******************* para mandar trama SET *****************
	******** criar a trama FLAG A C BCC FLAG
	******** enviar trama em vez do texto*/
	char SET[5];
	SET[0]=FLAG;
	SET[1]=A;
	SET[2]=C_SET;
	SET[3]=A^C_SET;
	SET[4]=FLAG;



	res = write(fd,SET,5);   
	printf("enviei trama SET! com %d bytes\n", res);
	/**************** para receber trama UA ****************/	
	char aux;
	res=-1;
	int itt=0;	
	//leitura da trama para buf
	while (1) 
	{
		while (res!=1)
        	res = read(fd,&aux,1); 
		res=-1;
		if(aux==FLAG)
		{
			buf[itt]=aux;
			itt++;
			aux=0x00;
			while(aux!=FLAG)
			{
				while (res!=1)
					res = read(fd,&aux,1); 
				res=-1;
				buf[itt]=aux;
				itt++;
			}
			if(buf[0]==FLAG && buf[1]==A && buf[2]==C_UA && buf[3]==A^C_UA && buf[4]==FLAG)
			{
				printf("recebi trama UA!\n");
				break;
			}
			printf("recebi trama errada!\n");
			break;
		}
	}	
	
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

	sleep(1);

   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }


    close(fd);
    return 0;
}
