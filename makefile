CC=gcc
CFLAGS=-I -lm
DEPS =  
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: Client Server

Client: exampleClient.c
	$(CC) -o Client exampleClient.c

Server: exampleServer.c  	
	$(CC) -o Server exampleServer.c 

clean:
	rm -rf *.o Server Client
