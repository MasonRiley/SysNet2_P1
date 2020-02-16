/* server.c: This program starts an http server that listens for connections.
 * It creates a socket, binds the IP and port for the socket, listens on the port,
 * accepts connections, sends data, then closes the socket.
 * 
 * Author(s): Mason Riley, Cesar Santiago
 * Course: COP4635
 * Project #: 1
 * Last Updated: 2/16/2020
 */

#include <stdio.h>          // Standard library
#include <stdlib.h>         // Standard library
#include <sys/socket.h>     // API and definitions for the sockets
#include <sys/types.h>      // more definitions
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>     // Structures to store address information
#include <unistd.h>         // Defines misc. symbolic constants and types
#include <string.h>         // String methods
#include <dirent.h>         // Directory & file search methods

#include "server.h"
#include "standards.h"      // Contains constants used in both server.c and client.c

#define IMAGE 1
#define HTML 2

char response[DATA_SIZE]; // Stores the entire response including header & payload
char files[256][256]; // Stores names of all files in cwd
int numFiles = 0; // Number of files in cwd

/*--------------------------------------HTTP headers used for responses------------------------------------*/
char* htmlHeader = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: %d\n\n";
char* imageHeader = "HTTP/1.1 200 OK\nContent-Type: image/png\nContent-Length: %lu\n\n";
char* errorHeader = "HTTP/1.1 404 Not Found\nContent-Type: text/html; charset=UTF-8\nContent-Length: %d\n\n";
/*---------------------------------------------------------------------------------------------------------*/

int main() {    

    long valread;
    char buff[BUFFER_SIZE];

    // Get list of all files in cwd
    getFiles();

    // Create the server socket using the socket function 
    int tcp_server_socket; // Variable for the socket descriptor 
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0); 

    // Define the server address
    struct sockaddr_in tcp_server_address; //declaring a structure for the address 
    tcp_server_address.sin_family = AF_INET;

    // Passing the port number, converting in right network byte order
    tcp_server_address.sin_port = htons(PORT_NUMBER); 

    // Connecting to 0.0.0.0
    tcp_server_address.sin_addr.s_addr = INADDR_ANY; 

    // Bind the socket to the IP address & port
    bind(tcp_server_socket, (struct sockaddr *) &tcp_server_address, 
            sizeof(tcp_server_address)); 

    // Listen for simultaneous connections
    listen(tcp_server_socket, MAX_CONN);
    printf("Server started, listening on port 60001.\n");

    while(1) {
        // Accept connections
        printf("Waiting for a connection...\n");
        int tcp_client_socket;
        tcp_client_socket = accept(tcp_server_socket, NULL, NULL); 
        printf("Connection successfully made.\n\n");

        // Receive requests from client
        valread = read(tcp_client_socket, buff, BUFFER_SIZE);
        printf("-----------CLIENT REQUEST----------\n%s\n", buff);
        printf("---------END CLIENT REQUEST--------\n\n");

        printf("----------SERVER RESPONSE----------\n");
        // Determine if requested file exists
        int fileIndex = checkFileExists(buff);
        if(fileIndex != -1 && valread > 0) {
            // If exists then determine file type, 
            // format data stream, and send it to client
            printf("Entered if\n"); 
            if(contentType(buff) == HTML) {
                // Read in HTML data
                printf("Reading in HTML file data...\n");
                readHTMLFile(files[fileIndex]);
                printf("...HTML file data successfully read.\n");

                // Send HTML data
                printf("Sending HTML file data to client...\n\n");
                printf("RESPONSE:\n%s\n", response);
                send(tcp_client_socket,response, sizeof(response), 0);
                printf("...HTML file data successfully sent.\n");
            }
            else if(contentType(buff) == IMAGE) {
                // Read in image data
                printf("Reading in image file data...\n");
                readImageFile(files[fileIndex]);
                printf("...Image data successfully read.\n");

                // Send image data
                printf("Sending image data to client...\n\n");
                printf("RESPONSE:\n%s\n", response);
                send(tcp_client_socket, response, sizeof(response), 0);
                printf("...Image data successfully sent.\n");
            }
        }
        else {
            // If not exists, then send 404 along with the 404 page
	        printf("ERROR: HTTP 404: Not Found\n\n");
            readErrorFile("404.html");
            printf("RESPONSE:\n%s\n", response);
            send(tcp_client_socket, response, sizeof(response), 0);
        }
        printf("--------END SERVER RESPONSE--------\n\n");
        
        // Reset all values for safety 
        memset(buff, 0, BUFFER_SIZE);
        memset(response, 0, sizeof(response));
        fileIndex = 0;
        valread = 0;
    }
    
    // Close the socket once done
    close(tcp_server_socket);

    return 0;
}

/**
 * readHTMLFile: Reads in the contents of a file one character at a time,
 * storing the results in dataBuff. Counts the number of chars for
 * Content-Length. Then concatenates the pre-formatted response header
 * (see line htmlHeader), the determined content-length, and the file
 * data to send to the client.
 * @Params fileName The name of the file being read in.
 */
void readHTMLFile(char *fileName) { 
    char ch;
    char dataBuff[DATA_SIZE] = "";
    FILE *fin;

    // Open HTML file, read each character into dataBuff, and track size
    fin = fopen(fileName, "r");
    int byteSize = 0;
    while((ch = fgetc(fin)) != EOF) {
        strncat(dataBuff, &ch, 1);
        ++byteSize;
    }
    
    // Copy appropriate header into 'response', then the data to be sent
    snprintf(response, sizeof(response), htmlHeader, byteSize);
    strcat(response, dataBuff);
    
    // Reset dataBuff
    memset(dataBuff, '\0', sizeof(dataBuff));
    
    // Close the file
    fclose(fin);
}

/**
 * readImageFile: Reads in the contents of an image as binary data,
 * storing the results in imgBuff. Determines the size of the image for
 * Content-Length. Then concatenates the pre-formatted response header
 * (see line imageHeader), the determined content-length, and the image
 * data to send to the client.
 * @Params fileName The name of the file being read in.
 */
void readImageFile(char *fileName) {
    FILE *fin;
    struct stat fileStats;
    unsigned char *imgBuff;
    int fd, headerLen;

    // Get size of image and open it
    fd = open(fileName, O_RDONLY);
    fstat(fd, &fileStats);
    fin = fopen(fileName, "rb");

    // Read image into 'imgBuff'
    imgBuff = malloc(fileStats.st_size);
    fread(imgBuff, sizeof(char), fileStats.st_size, fin);

    // Copy appropriate header into 'data', then memcpy image binary data into it
    headerLen = snprintf(response, sizeof(response), imageHeader, fileStats.st_size);
    memcpy(response + headerLen, imgBuff, fileStats.st_size);

    //Close file
    fclose(fin);
}

/**
 * readErrorFile: Reads in the contents of an error one character at a
 * time, storing the results in dataBuff. Counts the number of chars for
 * Content-Length. Then concatenates the pre-formatted response header
 * (see errorHeader), the determined content-length, and the file data to
 * send to the client.
 * @Params fileName The name of the file being read in.
 */
void readErrorFile(char *fileName) {
    char ch;
    char dataBuff[DATA_SIZE] = "";
    FILE *fin;
    int byteSize;

    // Open html file, read each character into dataBuff, and track size
    fin = fopen(fileName, "r");
    byteSize = 0;
    while((ch = fgetc(fin)) != EOF) {
        strncat(dataBuff, &ch, 1);
        ++byteSize;
    }

    // Copy appropriate header into data, then the data to be sent
    snprintf(response, sizeof(response), errorHeader, byteSize);
    strcat(response, dataBuff);

    //Reset dataBuff
    memset(dataBuff, '\0', sizeof(dataBuff));

    // Close the file
    fclose(fin);
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
            strcat(files[numFiles], file->d_name);
            ++numFiles;
        }
        closedir(directory);
    }
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
    printf("size of buffer = %d\n", strlen(buff));
    if((strlen(buff)) > 0) {
        char ch = buff[i + OFFSET];
   
        // If navigating to '/' 
        if(ch == ' ') {
            strcpy(fileName, "index.html");
        }
        // Otherwise, navigating to '/[value].html'
        else {
            while(ch != ' ') {
                fileName[i++] = ch;
                ch = buff[i + OFFSET];
            }
        }
     
        for(i = 0; i < numFiles; ++i) {
            if((strcmp(files[i], fileName)) == 0) {
                memset(fileName, 0, sizeof(fileName));
                return i;
            }
        }
    }
    memset(fileName, 0, sizeof(fileName));
    return -1;
}

int contentType(char* request)
{
    if(strstr(request, ".png")!= NULL)
    {
        return 1;
    }
    else if(strstr(request, ".html")!=NULL);
    {
        return 2;
    }
    return -1;
}

/*
char* parseRequest(char* request)
{
    char ch;
    int i, nameSize = 0;
    int reqSize =strlen(request);
    char* filename = "";
    for(i = 0; i < reqSize && ch != '/'; i++)
    {
        ch = request[i];
    }
    while(ch != ' ')
    {
        strncat(filename, &ch, 1);
        ch = request[i];
        nameSize++;
    }
    return filename;//Return 0 for size of zero if no file name could be parsed.
}*/

