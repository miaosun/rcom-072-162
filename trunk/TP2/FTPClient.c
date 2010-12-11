#include "FTPClient.h"


char * buf;
char * user;
char * pass;
char * addr;
char ** path;

int main(int argc, char *argv[])//o nome do servidor deve ser passado como parametro
{
	int socket_source, i=0, socket_dest;	//sockets source e destino

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
	
	
	if(parse_addr(argv[1])<0)
		return 0;
	printf("host: %s\n", addr);
	printf("user: %s\n", user);
	printf("pass: %s\n", pass);
	while(path[i]!=NULL)
	{
		printf("path %i: %s\n", i, path[i]);
		i++;		
	}
	
	//-------------ligar os sockets-------------------------
	printf("A ligar ao servidor %s\n", addr);
	socket_source = ligar(addr, 21);
	if (socket_source < 0) 
	{
		PRINT_ERROR("Error connecting server %s:21.\n", addr);
		return 0;
	} 
	else 
	{
		printf("FTP connection estabelecida em: %s:21\n", addr);
	}
	
	//FIM -------------- ligar os sockets ------------------- FIM
	
	//recebe mensagem de boas vindas
	recebe(socket_source);
	
	//envia utilizador
	snprintf(buf, MAX_MSG_LEN,"USER %s\n", user);
	write(socket_source, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	recebe(socket_source);
	
	//envia password
	snprintf(buf, MAX_MSG_LEN,"PASS %s\n", pass);
	write(socket_source, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	for(i=0; i<3; i++)
	{
		//printf("mensagem nr %i\n", i);
		recebe(socket_source);
		sleep(1);
	}
	
	//printf("saltou fora\n");
	
	//entra em modo passivo
	snprintf(buf, MAX_MSG_LEN,"PASV\n");
	write(socket_source, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	recebe(socket_source);
	
	socket_dest=con_pasv();
	
	cwd();
	write(socket_source, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	recebe(socket_source);
	
	//mostra o directorio onde esta
	snprintf(buf, MAX_MSG_LEN,"PWD\n");
	write(socket_source, buf, strlen(buf));
	PRINT_GREEN("<< %s", buf);
	recebe(socket_source);
	
	

	//deligar a ligação dos sockets
	disconnect(socket_source);
	disconnect(socket_dest);

	return 0;
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
		return -1;
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
	
		tokens=strstr(path[i-1], ".");
	//printf("tokens finais ponto: %s\n", tokens);
	if(i==0||tokens==NULL)
	{
		PRINT_ERROR("erro no path do ficheiro: tem que existir\n");
		return -1;
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
			return -1;
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
			return -1;
		}
		addr=aux;
	}

	return 1;
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
	char * tokens;
	int res;
	
	res=read(sock_fd,buf,MAX_MSG_LEN);
	if(res>0)
	{
		tokens=strtok(buf, "\n");
		tokens[res]="\0";
		PRINT_BLUE(">> %s\n", tokens);
		buf=tokens;
		return res;
	}
	else
	{
		PRINT_ERROR("nao conseguiu ler a mensagem do servidor!\n");
		return 0;
	}
}

void cwd(void)
{
	int i, size=0;
	
	strcpy(buf, "CWD ");
	
	while(path[size]!=NULL)
		size++;
	size--;
	
	//printf("size %i\n", size);
	
	for(i=0; i<size; i++)
	{
		//printf("buf %i: %s\n", i, buf);
		strcat(buf, "/");
		strcat(buf, path[i]);
	}
	strcat(buf, "\n");
	
	//printf("comando %s\n", buf);
	
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


/*int askCmd(char * buffer)//pede o comando e preenche-o no buffer
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

*/

