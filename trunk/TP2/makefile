CC = gcc
CFLAGS = -Wall

CLIENT = FTPClient.o
RECEPTOR = noncanonical.o

all: clean ftpclient

exec: ./ftpclient

ftpclient: $(CLIENT)
	$(CC) -o send $(EMISSOR)

FTPClient.o: FTPClient.c FTPClient.h
	$(CC) $(CFLAGS) $< -c -o $@


clean: 
	-rm $(CLIENT)
	-rm ftpclient
	clear
