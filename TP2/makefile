CC = gcc
CFLAGS = -Wall

CLIENT = FTPClient.o


all: clean ftpclient

exec: ./ftpclient

ftpclient: $(CLIENT)
	$(CC) -o ftpclient $(CLIENT)

FTPClient.o: FTPClient.c FTPClient.h
	$(CC) $(CFLAGS) $< -c -o $@


clean: 
	-rm $(CLIENT)
	-rm ftpclient
	clear
