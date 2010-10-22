CC = gcc
CFLAGS = -Wall

EMISSOR = writenoncanonical.o
RECEPTOR = noncanonical.o

all: clean send rece

emi: ./send /dev/ttyS0
rec: ./rece /dev/ttyS0


send: $(EMISSOR)
	$(CC) -o send $(EMISSOR)

writenoncanonical.o: writenoncanonical.c
	$(CC) $(CFLAGS) $< -c -o $@

rece: $(RECEPTOR)
	$(CC) -o rece $(RECEPTOR)

noncanonical.o: noncanonical.c
	$(CC) $(CFLAGS) $< -c -o $@

clean: 
	-rm $(EMISSOR)
	-rm $(RECEPTOR)
	-rm send
	-rm rece
	clear
