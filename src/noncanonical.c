//RECEPTOR


#include "noncanonical.h"

volatile int STOP=FALSE;
int mode;
char buf[255], ultimo_Ni;

int main(int argc, char** argv)
{
    int fd[2];
    struct termios oldtio,newtio;

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
		printf("abriu fifo de entrada\n\n");
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



	llopen(fd);


	//start
	int filedes;
	filedes=start(fd);
	if(filedes>0)
	{
		printf("criei ficheiro!\n");
		while(receive(fd, filedes)){}
	}

	llclose(fd);


	sleep(2);
	if(mode==SERIAL)
		tcsetattr(fd[0],TCSANOW,&oldtio);
	else 
		close(fd[1]);

	if(filedes>0)
		close(filedes);

	close(fd[0]);

    return 0;
}

int start(int fd[2])
{
	int res, aux;
	char pack[WORD_MAX], ca;
	int i;
	char filename[WORD_MAX], fn2[WORD_MAX];
	//printf("start\n");
	res=llread(fd, pack);
	while(res<0)
		res=llread(fd, pack);
	ca=pack[0];
	aux=atoi(&ca);
	printf("pack: %s\n", pack);
	printf("primeiro: %i\n", aux);
	if(aux==1)
	{
		printf("pacote start\n");
		ca=pack[1];
		aux=atoi(&ca);
		if(aux==1)
		{
			printf("pacote filename: ");
			ca=pack[2];
			aux=atoi(&ca);
			for(i=0; i<aux; i++)
				filename[i]=pack[3+i];
			if(3+i==res)
			{
				printf("%s\n", filename);
				sprintf(fn2, "copy_of_%s", filename);
				return open(fn2, O_RDWR | O_CREAT);
			}
		}
	}
	else
		return -1;
	return 0;
}

int receive(int fd[2], int filedes)
{
	int res, aux;
	char pack[WORD_MAX], ca;
	int i;

	res=llread(fd, pack);
	while(res<0)
		res=llread(fd, pack);
	//ca=pack[0];
	//aux=atoi(&ca);
	printf("primeiro: %d\n", aux);
	printf("tamanho: %d\n", res);
	if(aux=='0')
	{
		if(res==2)
		{
			write(filedes, pack[1], 1);
			printf("gravei no ficheiro!\n");
			return TRUE;
		}
	}
	else if(aux==3)//fim de transmissao
	{
		printf("fim de transmissao\n");
		return FALSE;
	}
	return FALSE;
}

int llopen(int fd[2])
{
	if(l_read(fd)<0) 
		printf("erro de leitura\n");
	if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_SET && buf[3]==(A_Snd_to_Rcv^C_SET) && buf[4]==FLAG)
	{
		printf("recebi trama SET!\n");
		envia_UA(fd);//envia UA
		ultimo_Ni=N1;
		return TRUE;
	}
	else
	{
		printf("recebi trama errada, esperava SET\n");
		return FALSE;
	}
}

int llread(int fd[2], char * buffer)
{
	int res, i, itt2=0;
	char BCC;
	//recebe trama I e envia RR
	res=l_read(fd);//le do ficheiro o
	if(res<0)
	{
		envia_REJ(fd);
		printf("erro de leitura\n");
		return -1;
	}
	else//leu correctamente
	{
		if(ultimo_Ni==N1)
			ultimo_Ni=N0;
		else
			ultimo_Ni=N1;
		

		if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==ultimo_Ni && buf[3]==(buf[1]^buf[2]) && buf[res-1]==FLAG)
		{//cabecalho correcto
			//printf("recebi trama I com %d bytes!\n", res);
			BCC=buf[4];
			for(i=4; i<res-2; i++)//depois de chegar aos dados, ha que trata-los
			{
				if(buf[i]==0x7d)//operacao de destuffing
				{
					printf("destuffing...\n");
					if(buf[i+1]==0x5e)
					{
						*(buffer+itt2)=FLAG;
						itt2++;
						if(i>4)
							BCC=BCC^FLAG;
					}
					else if(buf[i+1]==0x5d)
					{
						*(buffer+itt2)=0x7d;
						itt2++;
						if(i>4)
							BCC=BCC^0x7d;
					}
				}
				else
				{
					*(buffer+itt2)=buf[i];
					itt2++;
					if(i>4)
						BCC=BCC^buf[i];
				}
			}
			if(BCC==buf[res-2])
			{
				printf("recebi trama I com %d bytes\n", res);
				envia_RR(fd);
				return itt2;
			}
			else
			{
				envia_REJ(fd);//enviar REJ
				return -1;
			}
		}
		else
		{
			envia_REJ(fd);//enviar REJ
			return -1;
		}
	}
}

int llclose(int fd[2])
{
	int res;

	//recebe DISC
	if(l_read(fd)<0)
		printf("erro de leitura!\n");
	if(buf[0]==FLAG && buf[1]==A_Snd_to_Rcv && buf[2]==C_DISC && buf[3]==(A_Snd_to_Rcv^C_DISC) && buf[4]==FLAG) //se no buffer ja esta a trama toda comecando e acabando na FLAG, entao informa que recebeu e reenvia
	{
		printf("recebi trama DISC!\n");
		res=write(fd[1],buf,5); //envia trama DISC
		printf("enviei trama DISC! com %d bytes\n", res);
	}
	else
	{
		printf("recebi trama errada, esperava DISC\n");
		return FALSE;
	}

	//envia UA final
	if(l_read(fd)<0)
		printf("erro de leitura!\n");
	if(buf[0]==FLAG && buf[1]==A_Rcv_to_Snd && buf[2]==C_UA && buf[3]==(A_Rcv_to_Snd^C_UA) && buf[4]==FLAG)
	{
		printf("recebi trama UA!\n");
		printf("\nConclui com exito a transmissao de pacotes\n");
		return TRUE;
	}
	else
	{
		printf("recebi trama errada, esperava UA\n");
		return FALSE;
	}
}


void envia_UA(int fd[2])
{
	int res;
	buf[0]=FLAG;
	buf[1]=A_Snd_to_Rcv;
	buf[2]=C_UA;
	buf[3]=(A_Snd_to_Rcv^C_UA);
	buf[4]=FLAG;
	res=write(fd[1],buf,5);
	printf("enviei trama UA! com %d bytes\n", res);
}

void envia_RR(int fd[2])
{
	buf[0]=FLAG;
	buf[1]=A_Snd_to_Rcv;
	if(ultimo_Ni==N1)//parametro Nr 
		buf[2]=RR0;
	else
		buf[2]=RR1;
	buf[3]=buf[1]^buf[2];
	buf[4]=FLAG;
	write(fd[1],buf,5);
	printf("enviei trama RR!\n");
}

void envia_REJ(int fd[2])
{
	buf[0]=FLAG;
	buf[1]=A_Snd_to_Rcv;
	if(ultimo_Ni==N1)//parametro Nr 
		buf[2]=REJ0;
	else
		buf[2]=REJ1;
	buf[3]=buf[1]^buf[2];
	buf[4]=FLAG;
	write(fd[1],buf,5);
	printf("enviei trama REJ!\n");
}

int l_read(int fd[2])
{
	//vamos ler para buf
	char aux;
	int res;
	int itt;
	res=-1;
	itt=0;
	while(1)//espera pela primeira flag
	{
		while (res!=1)
    		res = read(fd[0],&aux,1); 
		res=-1;
		if(aux==FLAG)
		{
			buf[itt]=aux;
			break;
		}
	}
	while (res!=1)
		res = read(fd[0],&aux,1); //le o segundo byte
	res=-1;
	while(aux==FLAG)//enquanto for flag nao faz nada
	{
		while (res!=1)
			res = read(fd[0],&aux,1); 
		res=-1;
	}
	itt++;
	buf[itt]=aux;//deixou de ser flag, por isso armazena
	itt++;
	while(aux!=FLAG)//e os restantes tambem
	{
		while (res!=1)
			res = read(fd[0],&aux,1); 
		res=-1;
		buf[itt]=aux;
		itt++;
		if(itt>MAX_BYTES) return -1; //se ao fim de max_bytes ainda nao tiver encontrado uma flag, entao da erro
	}
	return itt;
}
