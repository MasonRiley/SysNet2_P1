// This is the example server code provided by Mishra
// Create Socket > bind the IP and port for the socket > listen on port > accept connection > send data > close socket
#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <string.h>
#include <dirent.h>

#include "exampleServer.h"

char responseHeader[1024] = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 59\n\n";
char files[256][256];
int numFiles = 0;

int main() {    
    char tcp_server_message[256] = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 59\n\n<!DOCTPYE html>\n<html>\n<body>\n<h1> HEADING </h1>\n</body>\n\n";

    //-----------------------------------------
    //-----1. Get list of all files in cwd-----
    //-----------------------------------------

    getFiles();

    //-------------------------------------
    //-----2. Create the server socket-----
    //-------------------------------------
    
    int tcp_server_socket; //variable for the socket descriptor
    
    /* Calling the socket function. 
     * Params: Domain of the socket (Internet in this case)
     *         Type of socket stream (TCP)
     *         Protocol (default, 0 for TCP) */ 
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0); 

    //--------------------------------------
    //-----4. Define the server address-----
    //--------------------------------------

    struct sockaddr_in tcp_server_address; //declaring a structure for the address
    
    // Structure fields' definition: Sets the address family of the 
    // address the client would connect to 
    tcp_server_address.sin_family = AF_INET;

    // Passing the port number, converting in right network byte order
    tcp_server_address.sin_port = htons(39756); 

    // Connecting to 0.0.0.0
    tcp_server_address.sin_addr.s_addr = INADDR_ANY; 

    //--------------------------------------------------------
    //-----5. Binding the socket to the IP address & port-----
    //--------------------------------------------------------

    /* Params: Which socket
     *         Cast for server address
     *         Its size */         
    bind(tcp_server_socket, (struct sockaddr *) &tcp_server_address, 
            sizeof(tcp_server_address)); 


    //------------------------------------------------
    //-----6. Listen for simultaneous connections-----
    //------------------------------------------------

    // Params: Which socket
    //         How many connections
    listen(tcp_server_socket, 5);

    // Server socket to interact with client, structure like before - if 
    // you know - else NULL for local connection
    int tcp_client_socket;

    checkFileExists("GET /index.html dadf");
    checkFileExists("GET /nope.html estsd");
    printf("Server started, waiting for connection...\n");
    tcp_client_socket = accept(tcp_server_socket, NULL, NULL); 
    printf("Connection successfully made.\n");
    while(1) {

        //-----------------------------
        //-----7. Send data stream-----
        //-----------------------------

        /* Params: Send where
         *         Send what
         *         How much
         *         Flags (optional) */
        char buff[30000] = {0};
        long valread = read(tcp_client_socket, buff, 30000);
        printf("%s\n", buff);
        
        if(valread > 0 && (checkFileExists(buff)) != -1) {
            readFile("example");
            printf("responseHeader: %s\n", responseHeader);
            printf("size of RH: %d\n", sizeof(responseHeader));
            send(tcp_client_socket, responseHeader, sizeof(responseHeader), 0);
        }
    }

    //-----------------------------
    //-----8. Close the socket-----
    //-----------------------------
    close(tcp_server_socket);

    return 0;
}

void readFile(char *fileName) {
    FILE *fin;
    //fin = fopen(fileName, "r");
    fin = fopen("index.html", "r");
    
    char ch;
    int byteSize = 0;
    while((ch = fgetc(fin)) != EOF) {
        ++byteSize;
        strncat(responseHeader, &ch, 1);
    }
    printf("bytesize = %d\n", byteSize);
}

void getFiles() {
    DIR *directory;
    struct dirent *file;
    directory = opendir(".");
    if(directory) {
        while((file = readdir(directory)) != NULL) {
            //files[numFiles][0] = '/';
            strcat(files[numFiles], file->d_name);
            printf("%s\n", files[numFiles]);
            ++numFiles;
        }
        closedir(directory);
    }
    printf("NUMFILES = %d\n", numFiles);
}

int checkFileExists(char *buff) {
    const int OFFSET = 5; //The first 5 characters of a request are 'GET /' 
    int i = 0;
    char fileName[256];
    char ch = buff[i + OFFSET];
    printf("FILE NAME = %s\n", fileName);
    while(ch != ' ') {
        fileName[i++] = ch;
        ch = buff[i + OFFSET];
    }
    printf("FILE NAME = %s\n", fileName);
    for(i = 0; i < numFiles; ++i) {
        if((strcmp(files[i], fileName)) == 0) {
            printf("FILE EXISTS\n");
            memset(fileName, 0, sizeof(fileName));
            return i;
        }
    }

    memset(fileName, 0, sizeof(fileName));
    printf("FILE DOES NOT EXIST\n");
    return -1;
}
