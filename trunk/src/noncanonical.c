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

#define FLAG 0x7E
#define A_Rcv_to_Snd 0x01
#define A_Snd_to_Rcv 0x03 
#define C_SET 0x03
#define C_UA 0x07

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

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

    newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
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

/****************** para receber trama e trata-la ****************/	
	//vamos usar buf
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
			if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_SET && buf[3]==A_Snd_to_Rcv^C_SET && buf[4]==FLAG)
			{
				printf("recebi trama SET!\n");
				buf[0]=FLAG;
				buf[1]=A_Snd_to_Rcv;
				buf[2]=C_UA;
				buf[3]=A_Snd_to_Rcv^C_UA;
				buf[4]=FLAG;
				res=write(fd,buf,5);
				printf("enviei trama UA! com %d bytes\n", res);
				break;
			}
		}
	}

	

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
