CC=gcc
CFLAGS=-I -lm
DEPS =  
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: Client Server

Client: client.c
	$(CC) -o Client client.c

Server: server.c  	
	$(CC) -o Server server.c 

clean:
	rm -rf *.o Server Client
