#include "FTPClient.h"
#include "getip.c"
#include "clientTCP.c"

#define SERVER_PORT 21
#define SERVER_ADDR "192.168.28.96"

char * line;
char ** cmds;

int main(int argc, char *argv[])//o nome do servidor deve ser passado como parametro
{
	int socket_source, socket_destino;	//sockets source e destino
	char * hostname = getHostName(*argv[0]);
	int validado = 0;
	
	//-------------ligar os sockets-------------------------
	printf("A ligar ao servidor 1\n");
	socketcsrc = connect(argv[1], 21);
	if (socket_source < 0) {
		printf("Error connecting server1.\n");
		return 0;
	} else {
		printf("FTP connection estabelecida em: %s\n", argv[1]);
	}
	
	printf("A ligar ao servidor 2\n");
	socketcdst = connect(argv[2], 21);
	if (socket_destino < 0) {
		printf("Erro na conexão ao servidor 2.\n");
		return 0;
	} else {
		printf("FTP connection estabelecida em: %s\n", argv[2]);
	}
	//FIM -------------- ligar os sockets ------------------- FIM
	
	
	do{
		validado = authenticate();
		if(validado == 0){
			printf("Autenticação errada, por favor, introduza os dados de novo.");
		}
	}while(validado==0);

	if(getFile() == 0){
		printf("O ficheiro não existe.");
	}

	//deligar a ligação dos sockets
	disconnect(socketc_source, socketc_destino);

	return 0;
}

int exec_cmd(char ** com)//executa o comando
{

	return 0;
}

int connect(char * hostname)//fazer a ligacao ao servidor, atravez de sockets, abrir canal de comunicacao com o servidor
{
	int	sockfd;
	struct	sockaddr_in server_addr;
	struct hostent *h;

	h = gethostbyname(hostname);
	if (h == NULL) {  
            herror("Erro a receber o IP");
            return -1;
    }
	
	serverIP = inet_ntoa(*((struct in_addr *) serverinfo->h_addr));

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
	
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        	perror("connect()");
			shutdown(socketc,SHUT_RDWR);
			close(socketc);
			return -1;
	}
	
	return sockfd;
}


int authenticate(void)//autenticar-se no servidor com sucesso, username e password
{

}


int getFile(char * filename)//fazer download do ficheiro
{

}


int disconnect(int socketc_source,int socketc_destino)//fechar a ligacao
{
	shutdown(socket_source,SHUT_RDWR);
	shutdown(socket_destino,SHUT_RDWR);
	close(socket_source);
	close(socket_destino);
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



