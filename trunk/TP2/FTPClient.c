#include "FTPClient.h"

char * line;
char ** cmds;

int main(int argc, char *argv[])//o nome do servidor deve ser passado como parametro
{
	line=malloc(MAX_CMD_LEN);
	cmds=(char**)malloc(MAX_CMD_LEN*sizeof(char));



	return 0;
}

int exec_cmd(char ** com)//executa o comando
{

	return 0;
}

int connect(char * hostname)//fazer a ligacao ao servidor, atravez de sockets, abrir canal de comunicacao com o servidor
{

}


int authenticate(void)//autenticar-se no servidor com sucesso, username e password
{

}


int getFile(char * filename)//fazer download do ficheiro
{

}


int disconnect(char * filename)//fechar a ligacao
{

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



