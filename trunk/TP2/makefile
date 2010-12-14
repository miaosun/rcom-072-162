CC = gcc
CFLAGS = -Wall

CLIENT = FTPClient.o


all: clean download

download: $(CLIENT)
	$(CC) -o download $(CLIENT)

FTPClient.o: FTPClient.c FTPClient.h
	$(CC) $(CFLAGS) $< -c -o $@


clean: 
	-rm $(CLIENT)
	-rm download
	clear
