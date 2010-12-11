#include "FTPClient.h"


char * line;
char ** cmds;

int main(int argc, char *argv[])//o nome do servidor deve ser passado como parametro
{
	int socket_source;	//sockets source e destino
	char * buf;
	
	if(argc!=4)
	{
		printf("usage: ./FTPClient server_address.domain.com\n");
		return 0;
	}
	
	//-------------ligar os sockets-------------------------
	printf("A ligar ao servidor %s\n", argv[1]);
	socket_source = ligar(argv[1], 21);
	if (socket_source < 0) 
	{
		PRINT_ERROR("Error connecting server %s.\n", argv[1]);
		return 0;
	} 
	else 
	{
		printf("FTP connection estabelecida em: %s\n", argv[1]);
	}
	
	//FIM -------------- ligar os sockets ------------------- FIM
	
	//recebe mensagem de boas vindas
	recebe(socket_source);
	
	//envia utilizador
	sprintf(buf,"USER %s\n", argv[2]);
	write(socket_source, buf, strlen(buf));
	recebe(socket_source);
	
	//envia password
	sprintf(buf,"PASS %s\n", argv[3]);
	write(socket_source, buf, strlen(buf));
	recebe(socket_source);
	

	//deligar a ligação dos sockets
	disconnect(socket_source);

	return 0;
}



int ligar(char * hostname, int port)//fazer a ligacao ao servidor, atravez de sockets, abrir canal de comunicacao com o servidor
{
	int	sockfd;
	struct	sockaddr_in server_addr;
	struct hostent *h;
	char * serverIP;

	h = gethostbyname(hostname);
	if (h == NULL) {  
            herror("Erro a receber o IP");
            return -1;
    }
	
	serverIP = inet_ntoa(*((struct in_addr *) h->h_addr));
	
	printf("IP: %s\n", serverIP);

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(serverIP);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
	
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        	perror("connect()");
			disconnect(sockfd);
			return -1;
	}
	
	return sockfd;
}

int recebe(int sock_fd)
{
	char buffer[MAX_MSG_LEN];
	char * tokens;
	int res;
	
	res=read(sock_fd,buffer,255);
	if(res>0)
	{
		tokens=strtok(buffer, "\n");
		tokens[res]="\0";
		PRINT_BLUE(">>%s\n", tokens);
		return 0;
	}
	else
	{
		PRINT_ERROR("nao conseguiu ler a mensagem do servidor!\n");
		return 0;
	}
}

int authenticate(void)//autenticar-se no servidor com sucesso, username e password
{
	return 0;
}


int getFile(char * filename)//fazer download do ficheiro
{
	return 0;
}


int disconnect(int socket_fd)//fechar a ligacao
{
	//shutdown(socket,SHUT_RDWR);
	close(socket_fd);
	close(socket_fd);
	return 0;
}


int askCmd(char * buffer)//pede o comando e preenche-o no buffer
{
	printf(">> ");
	scanf("%s", buffer);
	return 0;
}


int parseCmd(char * buffer)//interpreta o comando e preenche a o array de comandos
{
	char* tokens;
	int itt = 0;

	tokens=strtok(buffer," ");
	while (tokens != NULL) {
		cmds[itt] = tokens;		
		tokens = strtok (NULL, " ");
		itt++;			
	}
	cmds[itt] = NULL;

	//printf(">> formatou linha comandos\n");
	return 0;
}


void clear_last_cmd(char** com) //limpa o ultimo comando armazenado em cmds
{
	int itt;
	for(itt=0;com[itt] != NULL;itt++)
		com[itt] = NULL;
	//printf(">> limpou linha comandos\n");
}

int exec_cmd(char ** com)//executa o comando
{

	return 0;
}



