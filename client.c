/** 
 * client.c: This program runs a client which connects to a running server.
 * It creates the socket, connects to the server, receives data, displays
 * the data, and then closes the connection.
 *
 * Author(s): Mason Riley, Cesar Santiago
 * Course: COP4635
 * Project #: 1
 * Last Updated: 2/16/2020
 */

#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <unistd.h>         //Defines misc. symbolic constants and types
#include <string.h>         //String methods
#include "standards.h"      //Contains constants used in both server.c and client.c


int main(){    
    //------------------------------------
    //-----1. Creating the TCP socket-----
    //------------------------------------

    // Socket descriptor
    int tcp_client_socket;        

    /* Calling the socket function
     * Params: Socket domain
     *         Socket stream type
     *         TCP protocol (default) */
    tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0);

    //----------------------------------------------------------
    //-----2. Specify address and port of the remote socket-----
    //----------------------------------------------------------
    
    // Declaring a structure for the address    
    struct sockaddr_in tcp_server_address;             
    
    // Structure Fields' definition: Sets the address family 
    // of the address the client would connect to    
    tcp_server_address.sin_family = AF_INET;
    
    // Specify and pass the port number to connect - converting in right network byte order    
    tcp_server_address.sin_port = htons(PORT_NUMBER);   
    
    // Connecting to 0.0.0.0
    tcp_server_address.sin_addr.s_addr = INADDR_ANY;       
    
    //--------------------------------------------
    //-----3. Connecting to the remote socket-----
    //--------------------------------------------

    /* Params: Which socket 
     *         Cast for address to the specific structure type 
     *         Size of address */
    int connection_status = connect(tcp_client_socket, 
            (struct sockaddr *) &tcp_server_address, sizeof(tcp_server_address));         

    // Return value of 0 means all okay, -1 means a problem        
    if (connection_status == -1){                                                  
        printf(" Problem connecting to the socket! Sorry!! \n");     
    }  

    char *test = "GET /index.html /HTML1.1";
    send(tcp_client_socket, test, strlen(test), 0);
    char tcp_server_response[256];    

    /* Params: Where (socket)
     *         What (string)
     *         How much (size of the server response)
     *         Flags (0) */
    recv(tcp_client_socket, &tcp_server_response, sizeof(tcp_server_response), 0); 
    
    //------------------------------------------------
    //-----4. Output, as received from the server-----
    //------------------------------------------------
    printf("\n\n Server says: %s \n", tcp_server_response);    
    
    //-------------------------
    //-----5. Close socket-----
    //-------------------------
    close(tcp_client_socket);    
    return 0;
}
