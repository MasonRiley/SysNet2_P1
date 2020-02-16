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
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>     //Structures to store address information
#include <unistd.h>         //Defines misc. symbolic constants and types
#include <string.h>         //String methods
#include <dirent.h>         //Directory & file search methods

#include "server.h"
#include "standards.h"

#define IMAGE 1
#define HTML 2

char* responseHeader = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: %d\n\n";
char* imageResponseHeader = "HTTP/1.1 200 OK\nContent-Type: image/png\nContent-Length: %lu\n\n";
char* errorHeader = "HTTP/1.1 404 Not Found\nContent-Type: text/html; charset=UTF-8\nContent-Length: %d\n\n";
char data[DataSize];
char files[256][256];
int numFiles = 0;

int main() {    

    long valread;
    char buff[BufferSize];

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
    tcp_server_address.sin_port = htons(PortNumber); 

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
    listen(tcp_server_socket, MaxConnections);
    printf("Server started, listening on port 60001.\n");

    while(1) {
        // Server socket to interact with client, structure like before - if 
        // you know - else NULL for local connection
        printf("Waiting for a connection...\n");
        int tcp_client_socket;
        tcp_client_socket = accept(tcp_server_socket, NULL, NULL); 
        printf("Connection successfully made.\n");


        //-----------------------------
        //-----7. Send data stream-----
        //-----------------------------


        /* Params: Send where
         *         Send what
         *         How much
         *         Flags (optional) */
        valread = read(tcp_client_socket, buff, BufferSize);
        printf("---DEBUG request read---:\n%s\n", buff); //DEBUG STRING - remove when done. Prints client request string

        //Determine if requested html file exists
        int fileIndex = checkFileExists(buff);
        if(fileIndex != -1 && valread > 0) {
            //If so, retrieve file data and format response header appropriately 
            //Then send formatted response back to client 
            printf("Sending HTTP 200 OK\n");
            
            if(contentType(buff) == HTML) {
                // Read in HTML data
                printf("Reading in HTML file data...\n");
                readTextFile(files[fileIndex]);
                printf("...HTML file data successfully read.\n");

                // Send HTML data
                printf("Sending HTML file data to client...\n");
                send(tcp_client_socket, data, sizeof(data), 0);
                printf("...HTML file data successfully sent.\n");
            }
            else if(contentType(buff) == IMAGE) {
                // Read in image data
                printf("Reading in image file data...\n");
                readImageFile(files[fileIndex]);
                printf("...Image data successfully read.\n");

                //Send image data
                printf("Sending image data to client...\n");
                send(tcp_client_socket, data, sizeof(data), 0);
                printf("...Image data successfully sent.\n");
            }
        }
        else {
	        printf("Sending HTTP 404: Not Found\n");
            readErrorFile("404.html");
            send(tcp_client_socket, data, sizeof(data), 0);
        }
        
        // Reset all values for safety 
        memset(buff, 0, BufferSize);
        memset(data, 0, sizeof(data));
        fileIndex = 0;
        valread = 0;
    }

    //-----------------------------
    //-----8. Close the socket-----
    //-----------------------------
    close(tcp_server_socket);

    return 0;
}

/**
 * readTextFile: Reads in the contents of a file one character at a time,
 * storing the results in dataBuff. Counts the number of chars for
 * Content-Length. Then concatenates the pre-formatted response header
 * (see line 19), the determined content-length, and the file data to
 * send to the client.
 * @Params fileName The name of the file being read in.
 */
void readTextFile(char *fileName) { 
    char ch;
    char dataBuff[DataSize] = "";
    FILE *fin;

    // Open html file, read each character into dataBuff, and track size
    fin = fopen(fileName, "r");
    int byteSize = 0;
    while((ch = fgetc(fin)) != EOF) {
        strncat(dataBuff, &ch, 1);
        ++byteSize;
    }
    
    // Copy appropriate header into 'data', then the data to be sent
    snprintf(data, sizeof(data), responseHeader, byteSize);
    strcat(data, dataBuff);
    
    // Reset dataBuff
    memset(dataBuff, '\0', sizeof(dataBuff));
    
    // Close the file
    fclose(fin);
}

void readImageFile(char *fileName) {
    FILE *fin;
    struct stat fileStats;
    unsigned char *img;
    int fd, headerLen;

    // Get size of image and open it
    fd = open(fileName, O_RDONLY);
    fstat(fd, &fileStats);
    fin = fopen(fileName, "rb");

    // Read image into 'img'
    img = malloc(fileStats.st_size);
    fread(img, sizeof(char), fileStats.st_size, fin);

    // Copy appropriate header into 'data', then memcpy image binary data into it
    headerLen = snprintf(data, sizeof(data), imageResponseHeader, fileStats.st_size);
    memcpy(data + headerLen, img, fileStats.st_size);

    //Close file
    fclose(fin);
}

void readErrorFile(char *fileName) {
    char ch;
    char dataBuff[DataSize] = "";
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
    snprintf(data, sizeof(data), errorHeader, byteSize);
    strcat(data, dataBuff);

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
}

