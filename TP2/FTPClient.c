#include "FTPClient.h"


char * buf;
char * user;
char * pass;
char * addr;
char ** path;
int path_size;
int main_socket;
int data_socket;

int main(int argc, char *argv[])//o nome do servidor deve ser passado como parametro
{
	int res;

	buf=malloc(MAX_MSG_LEN);
	user=malloc(MAX_MSG_LEN);
	pass=malloc(MAX_MSG_LEN);
	addr=malloc(MAX_MSG_LEN);
	path=(char **)malloc(MAX_WORD_LEN * sizeof(char));
	
	if(argc!=2)
	{
		printf("usage: ./FTPClient ftp://[<user>:<password>@]<host>/<url-path>\n");
		return 0;
	}
	
	if(parse_addr(argv[1])!=TRUE)
	{
		printf("usage: ./FTPClient ftp://[<user>:<password>@]<host>/<url-path>\n");
		return 0;
	}
	
	
	res=exec();
	
	if(res==1)
		disconnect(main_socket);
	else if(res==2)
	{
		disconnect(main_socket);
		disconnect(data_socket);
	}

	return 0;
}

int exec(void)
{
	int res, i=0;
	
	//printf("host: %s\n", addr);
	//printf("user: %s\n", user);
	//printf("pass: %s\n", pass);
	/*while(path[i]!=NULL)
	{
		printf("path %i: %s\n", i, path[i]);
		i++;		
	}*/

	printf("A ligar ao servidor %s...\n", addr);
	main_socket = ligar(addr, 21);
	if (main_socket < 0) 
	{
		PRINT_ERROR("Error connecting server %s:21.\n", addr);
		return 0;
	} 
	else 
	{
		printf("FTP connection estabelecida em: %s:21, a espera de resposta...\n", addr);
	}
	
	//recebe mensagem de boas vindas
	res=recebe(main_socket);
	if(res>3)
	{
		PRINT_ERROR("Servidor com resposta negativa, exiting...\n");
		return 1;
	}
	
	//envia utilizador
	snprintf(buf, MAX_MSG_LEN,"USER %s\r\n", user);
	write(main_socket, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	res=recebe(main_socket);
	if(res>3)
	{
		PRINT_ERROR("Servidor com resposta negativa, exiting...\n");
		return 1;
	}
	
	//envia password
	snprintf(buf, MAX_MSG_LEN,"PASS %s\r\n", pass);
	write(main_socket, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	for(i=0; i<3; i++)
	{
		//printf("mensagem nr %i\n", i);
		res=recebe(main_socket);
		if(res>3)
		{
			PRINT_ERROR("Servidor com resposta negativa, exiting...\n");
			return 1;
		}
		sleep(1);
	}
	
	//printf("saltou fora\n");
	
	//entra em modo passivo
	snprintf(buf, MAX_MSG_LEN,"PASV\r\n");
	write(main_socket, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	res=recebe(main_socket);
	if(res>3)
	{
		PRINT_ERROR("Servidor com resposta negativa, exiting...\n");
		return 1;
	}
	
	data_socket=con_pasv();
	
	if(data_socket<0)
	{
		PRINT_ERROR("Problema ao abrir a socket de dados, exiting\n");
		return 1;
	}
	
	
	//printf("abriu socket de dados\n");
	
	retr();
	write(main_socket, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	recebe_ficheiro(data_socket);
	
	return  -1;
}

int recebe_ficheiro(int sock_fd)
{
	int fd, res;
	
	char buffer[MAX_MSG_LEN];
	
	fd=open(path[path_size-1], O_CREAT | O_WRONLY | O_EXCL);
	
	if(fd<0)
	{
		PRINT_ERROR("Erro a abrir ficheiro local para escrita, exiting...\n");
		return FALSE;
	}
		
	printf("A transferir ficheiro %s...\n", path[path_size-1]);
	
	res=recv(sock_fd, buffer, MAX_MSG_LEN, MSG_WAITALL);
	while(res>0)
	{
		write(fd, buffer, res);
		res=recv(sock_fd, buffer, MAX_MSG_LEN, MSG_WAITALL);
	}
	
	close(fd);
	
	printf("SUCESSO!\n");
	
	return TRUE;
}

void retr(void)
{
	int i=0;
	
	strcpy(buf, "RETR ");
		
	
	while(path[i]!=NULL)
	{
		//printf("buf %i: %s\n", i, buf);
		strcat(buf, "/");
		strcat(buf, path[i]);
		i++;
	}
	strcat(buf, "\r\n");
	
	//printf("comando %s\n", buf);
}

int parse_addr(char * buffer)
{
	char * tokens; 
	char * aux;
	char * host;
	int i=0;
	
	tokens=strtok(buffer, "/");
	if(strcmp(tokens, "ftp:")!=0)
	{
		PRINT_ERROR("erro no url. usage: ftp://[<user>:<password>@]<host>/<url-path>\n");
		return FALSE;
	}
	host=strtok(NULL, "/");
	//printf("host %s\n", host);
	
	tokens=strtok(NULL, "/");
	while(tokens!=NULL)
	{
		path[i]=tokens;
		//printf("path %i: %s\n", i, path[i]);
		tokens=strtok(NULL, "/");
		i++;
	}
	path[i]=NULL;
	
	path_size=i;
	
		tokens=strstr(path[i-1], ".");
	//printf("tokens finais ponto: %s\n", tokens);
	if(i==0||tokens==NULL)
	{
		PRINT_ERROR("erro no path do ficheiro: tem que existir\n");
		return FALSE;
	}
	
	aux=strstr(host, ":");
		
	if(aux==NULL)
	{
		//printf("nao tem ':' \n");
		user="anonymous";
		pass="anonymous@anonymous.com";
		aux=strstr(host, "@");
		if(aux==NULL)
		{
			//printf("nao tem '@' \n");
			addr=host;
		}
		else
		{
			PRINT_ERROR("erro na especificacao do utilizador e password\n");
			return FALSE;
		}
	}
	else
	{
		//printf("tem ':' \n");
		
		aux=strtok(host, ":");
		//printf("antes de ':' %s\n", aux);
		
		user=aux;
		aux=strtok(NULL, "@");
		//printf("tem '@' \n");
		pass=aux;
		aux=strtok(NULL, "/");
		if(aux==NULL)
		{
			//printf("nao tem '@' \n");
			PRINT_ERROR("erro na especificacao do utilizador e password\n");
			return FALSE;
		}
		addr=aux;
	}

	return TRUE;
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
	
	//printf("IP: %s\n", serverIP);

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
	//printf("ligou na porta %i\n", port);
	
	return sockfd;
}

int recebe(int sock_fd)
{
	char * msg;
	char response[2], response_t[4];
	int res, i_response;
	
	res=read(sock_fd,buf,MAX_MSG_LEN);
	if(res>0)
	{
		msg=strtok(buf, "\n");
		msg[res]='\0';
		PRINT_BLUE(">> %s\n", msg);
		buf=msg;
		strncpy(response_t, msg, 3);
		response[0]=msg[0];
		response[1]='\0';
		i_response=atoi(response);
		//printf("codigo %i\n", res);
		
		/*if(msg[3]=='-')
		{
			do
			{
				res=read(sock_fd,buf,MAX_MSG_LEN);
				printf("dentro\n");
				msg=strtok(buf, "\n");
				msg[res]='\0';
				PRINT_BLUE(">> %s\n", msg);				
			}
			while(strstr(buf, response_t)==NULL);
		}
		
		printf("saiu\n");
		*/
		
		return i_response;
	}
	else
	{
		PRINT_ERROR("nao conseguiu ler a mensagem do servidor!\n");
		return 0;
	}
}

int con_pasv(void)
{
	int i1, i2, port;
	char * tokens;
	
	tokens=strtok(buf, ",");
	tokens=strtok(NULL, ",");
	tokens=strtok(NULL, ",");
	tokens=strtok(NULL, ",");
	tokens=strtok(NULL, ",");
	i1=atoi(tokens);
	//printf("parte 1: %i\n", i1);
	tokens=strtok(NULL, ",");
	i2=atoi(tokens);
	//printf("parte 2: %i\n", i2);
	
	port=i1*256+i2;
	
	return ligar(addr, port);
}

int disconnect(int socket_fd)//fechar a ligacao
{
	shutdown(socket_fd,SHUT_RDWR);
	close(socket_fd);
	return 0;
}
