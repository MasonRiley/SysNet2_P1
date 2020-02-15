//Basic TCP Client: socket() creation > connect() > receive() > display > close
#include <stdio.h>          //Standard library
#include <stdlib.h>         //Standard library
#include <sys/socket.h>     //API and definitions for the sockets
#include <sys/types.h>      //more definitions
#include <netinet/in.h>     //Structures to store address information
#include <string.h>

int main(){    
    //---------------------------------
    //-----Creating the TCP socket-----
    //---------------------------------

    // Socket descriptor
    int tcp_client_socket;        

    /* Calling the socket function
     * Params: Socket domain
     *         Socket stream type
     *         TCP protocol (default) */
    tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0);

    //-------------------------------------------------------
    //-----Specify address and port of the remote socket-----
    //-------------------------------------------------------
    
    // Declaring a structure for the address    
    struct sockaddr_in tcp_server_address;             
    
    // Structure Fields' definition: Sets the address family 
    // of the address the client would connect to    
    tcp_server_address.sin_family = AF_INET;
    
    // Specify and pass the port number to connect - converting in right network byte order    
    tcp_server_address.sin_port = htons(39756);   
    
    // Connecting to 0.0.0.0
    tcp_server_address.sin_addr.s_addr = INADDR_ANY;       
    
    //-----------------------------------------
    //-----Connecting to the remote socket-----
    //-----------------------------------------

    /* Params: Which socket 
     *         Cast for address to the specific structure type 
     *         Size of address */
    int connection_status = connect(tcp_client_socket, 
            (struct sockaddr *) &tcp_server_address, sizeof(tcp_server_address));         

    // Return value of 0 means all okay, -1 means a problem        
    if (connection_status == -1){                                                  
        printf(" Problem connecting to the socket! Sorry!! \n");     
    }  

    char *test = "Hello from the client!\n";
    send(tcp_client_socket, test, strlen(test), 0);
    char tcp_server_response[256];    

    /* Params: Where (socket)
     *         What (string)
     *         How much (size of the server response)
     *         Flags (0) */
    recv(tcp_client_socket, &tcp_server_response, sizeof(tcp_server_response), 0); 
    
    //---------------------------------------------
    //-----Output, as received from the server-----
    //---------------------------------------------
    printf("\n\n Server says: %s \n", tcp_server_response);    
    
    //----------------------
    //-----Close socket-----
    //----------------------
    close(tcp_client_socket);    
    return 0;
}
