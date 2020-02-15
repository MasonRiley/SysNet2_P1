// This is the example server code provided by Mishra
// Create Socket > bind the IP and port for the socket > listen on port > accept connection > send data > close socket
#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
int main() {    
    char tcp_server_message[256] = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 36\n\n<body>\n<h1> HEADING </h1>\n</body>\n\n";
    //char tcp_server_message[256] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 59\n\n<!DOCTPYE html>\n<html>\n<body>\n<h1> HEADING </h1>\n</body>\n\n";
    //char tcp_server_message[256] = " Hello, I am the TCP Server you successfully connected to!! \n\n               Bye Bye!!!\n\n";

    //----------------------------------
    //-----Create the server socket-----
    //----------------------------------
    
    int tcp_server_socket; //variable for the socket descriptor
    
    /* Calling the socket function. 
     * Params: Domain of the socket (Internet in this case)
     *         Type of socket stream (TCP)
     *         Protocol (default, 0 for TCP) */ 
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0); 

    //-----------------------------------
    //-----Define the server address-----
    //-----------------------------------

    struct sockaddr_in tcp_server_address; //declaring a structure for the address
    
    // Structure fields' definition: Sets the address family of the 
    // address the client would connect to 
    tcp_server_address.sin_family = AF_INET;

    // Passing the port number, converting in right network byte order
    tcp_server_address.sin_port = htons(39756); 

    // Connecting to 0.0.0.0
    tcp_server_address.sin_addr.s_addr = INADDR_ANY; 

    //-----------------------------------------------------
    //-----Binding the socket to the IP address & port-----
    //-----------------------------------------------------

    /* Params: Which socket
     *         Cast for server address
     *         Its size */         
    bind(tcp_server_socket, (struct sockaddr *) &tcp_server_address, 
            sizeof(tcp_server_address)); 


    //---------------------------------------------
    //-----Listen for simultaneous connections-----
    //---------------------------------------------

    // Params: Which socket
    //         How many connections
    listen(tcp_server_socket, 5);

    // Server socket to interact with client, structure like before - if 
    // you know - else NULL for local connection
    int tcp_client_socket;

    printf("Server started, waiting for connection...\n");
    while(1) {
        tcp_client_socket = accept(tcp_server_socket, NULL, NULL); 
        printf("Connection successfully made.\n");

        //--------------------------
        //-----Send data stream-----
        //--------------------------

        /* Params: Send where
         *         Send what
         *         How much
         *         Flags (optional) */
        char buff[30000] = {0};
        long valread = read(tcp_client_socket, buff, 30000);
        printf("%s\n", buff);
        //write(tcp_client_socket, tcp_server_message, sizeof(tcp_server_message));
        send(tcp_client_socket, tcp_server_message, sizeof(tcp_server_message), 0);
    }

    // Close the socket
    close(tcp_server_socket);

    return 0;
}

