CC=gcc
CFLAGS=-I -lm -Wall -G
DEPS =  
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: httpClient httpServer

httpClient: httpClient.c
	$(CC) -o httpClient httpClient.c

httpServer: httpServer.c  	
	$(CC) -o httpServer httpServer.c 

clean:
	rm -rf *.o httpServer httpClient
