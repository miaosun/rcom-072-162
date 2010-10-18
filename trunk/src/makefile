CC = gcc
CFLAGS = -Wall

EMISSOR = writenoncanonical.o
RECEPTOR = noncanonical.o

all: clean send rec

send: $(EMISSOR)
	$(CC) -o send $(EMISSOR)

writenoncanonical.o: writenoncanonical.c
	$(CC) $(CFLAGS) $< -c -o $@

rec: $(RECEPTOR)
	$(CC) -o rec $(RECEPTOR)

noncanonical.o: noncanonical.c
	$(CC) $(CFLAGS) $< -c -o $@

clean: 
	-rm $(EMISSOR)
	-rm $(RECEPTOR)
	-rm send
	-rm rec
	clear
