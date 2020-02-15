/* server.c: This program starts an http server that listens for connections.
 * It creates a socket, binds the IP and port for the socket, listens on the port,
 * accepts connections, sends data, then closes the socket.
 * 
 * Author(s): Mason Riley, Cesar Santiago
 * Project #: 1
 * Last Updated: 2/15/2020
 */
#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <string.h>         //String methods
#include <dirent.h>         //Directory & file search methods

#include "server.h"

char responseHeader[1024] = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: ";
char data[1024];
char files[256][256];
int numFiles = 0;

int main() {    
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
    printf("Server started, waiting for connection...\n");

    // Server socket to interact with client, structure like before - if 
    // you know - else NULL for local connection
    int tcp_client_socket;
    tcp_client_socket = accept(tcp_server_socket, NULL, NULL); 
    printf("Connection successfully made.\n");

    /*~~~~~~~~~~~~~~~FROM HERE TO~~~~~~~~~~~~~~~~~~*/
    checkFileExists("GET /index.html dadf");
    checkFileExists("GET /nope.html estsd");
    readFile("index.html");
    /*~~~~~~~~~~~~~~HERE ARE TESTS~~~~~~~~~~~~~~~~~*/

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
        printf("%s\n", buff); //DEBUG STRING - remove when done. Prints client request string

        //Determine if requested html file exists
        int fileIndex = checkFileExists(buff);
        if(valread > 0 && fileIndex != -1) {
            //If so, retrieve file data and format response header appropriately 
            memset(data, 0, sizeof(data)); //Reset data from prior uses
            readFile(files[fileIndex]);

            //Then send formatted response back to client 
            send(tcp_client_socket, data, sizeof(data), 0);
        }
    }

    //-----------------------------
    //-----8. Close the socket-----
    //-----------------------------
    close(tcp_server_socket);

    return 0;
}

/**
 * readFile: Reads in the contents of a file one character at a time,
 * storing the results in dataBuff. Counts the number of chars for
 * Content-Length. Then concatenates the pre-formatted response header
 * (see line 19), the determined content-length, and the file data to
 * send to the client.
 * @Params fileName The name of the file being read in.
 */
void readFile(char *fileName) { 
    char ch;
    char dataBuff[1024] = "";
    char sizeBuff[10];
    FILE *fin;
    fin = fopen(fileName, "r");
    int byteSize = 0;
    while((ch = fgetc(fin)) != EOF) {
        ++byteSize;
        strncat(dataBuff, &ch, 1);
    }
    sprintf(sizeBuff, "%d", byteSize);
    strcat(sizeBuff, "\n\n");
    strcat(data, responseHeader);
    strcat(data, sizeBuff);
    strcat(data, dataBuff);
    printf("data: %s\n", data);
    printf("bytesize = %d\n", byteSize);
    memset(dataBuff, '\0', sizeof(dataBuff));
    memset(sizeBuff, '\0', sizeof(sizeBuff));
}

/**
 * getFiles: Reads in every file in the cwd and stores their names in
 * 'files'. Additionally tracks how many files are in the cwd.
 */
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

/**
 * checkFileExists: Takes in a complete request header from the client  
 * and determines if the requested file from the client exists. If it 
 * exists, returns the index of the file in 'files', -1 otherwise.
 * @Params buff The buffer containing the entire request header
 * @Return the index of the file in 'files' if the file exists, -1
 * otherwise
 */
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
