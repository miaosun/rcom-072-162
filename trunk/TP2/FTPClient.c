#include "FTPClient.h"

char * line;
char ** cmds;

int main(int argc, char *argv[])
{
	line=malloc(MAX_CMD_LEN);
	cmds=(char**)malloc(MAX_CMD_LEN*sizeof(char));


}

int askCmd(char * buffer)//pede o comando e preenche-o no buffer
{

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


void clear_last_cmd(char** com) 
{
	int itt;
	for(itt=0;com[itt] != NULL;itt++)
		com[itt] = NULL;
	//printf(">> limpou linha comandos\n");
}



