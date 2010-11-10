CC = gcc
CFLAGS = -Wall

EMISSOR = writenoncanonical.o
RECEPTOR = noncanonical.o

all: clean send rece

emi: ./send /dev/ttyS0
rec: ./rece /dev/ttyS0


send: $(EMISSOR)
	$(CC) -o send $(EMISSOR)

writenoncanonical.o: writenoncanonical.c writenoncanonical.h
	$(CC) $(CFLAGS) $< -c -o $@

rece: $(RECEPTOR)
	$(CC) -o rece $(RECEPTOR)

noncanonical.o: noncanonical.c noncanonical.h
	$(CC) $(CFLAGS) $< -c -o $@

clean: 
	-rm $(EMISSOR)
	-rm $(RECEPTOR)
	-rm send
	-rm rece
	-rm E_2_R
	-rm R_2_E
	-rm copy_of_1.jpg
	clear
