#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <ctype.h>
#include <iostream>

#include "helperFcns.h"

using namespace std;

//Prints out the server address and port number
void print_server_info(int socketfd, struct sockaddr_in *my_addr)
{
    //Get port
    socklen_t channellen = sizeof(*my_addr);
    sockaddr_in my_addr1;
    getsockname(socketfd, (sockaddr*) &my_addr1, &channellen);
    unsigned short portno = ntohs(my_addr1.sin_port);

    //Get address
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    cout << "SERVER_ADDRESS " << hostname << endl;
    cout << "SERVER_PORT " << portno << endl;
}

void convert_to_titlecase(char* str)
{
    for(int i = 0; i < strlen(str); i++)
    {
        if (i - 1 == -1 || (i-1 >= 0 && str[i-1] == ' '))
        {
            str[i] = toupper(str[i]);
        } 
        else
        {
            str[i] = tolower(str[i]);
        }
    }
}

int main () {
    const int max_clients = 5;
    struct sockaddr_in my_addr, client_addr;
    int listen_socketfd, incoming_socketfd, max_socketfd, client_socket[max_clients];
    fd_set socketfd_set;
      
    //initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < max_clients; i++) 
    {
        client_socket[i] = 0;
    }
    
    //Create a socket to listen for incoming connections
    listen_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_socketfd == -1)
        handle_error("socket");

    //Bind the listening socket to a free port
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listen_socketfd, (sockaddr *) &my_addr, sizeof(my_addr)))
        handle_error("bind");

    //Listen for incoming connections
    if(listen(listen_socketfd, max_clients) == -1)
        handle_error("listen");

    print_server_info(listen_socketfd, &my_addr);
     
    //Server runs forever
    while(true) 
    {
        //clear the socket set
        FD_ZERO(&socketfd_set);
  
        //add listening socket to the set
        FD_SET(listen_socketfd, &socketfd_set);
        max_socketfd = listen_socketfd;
      
        //add connected client sockets to set
        for (int i = 0; i < max_clients; i++) 
        {
            int sd = client_socket[i];
             
            //if socket has been bound, add it to the set
            if(sd > 0)
                FD_SET(sd, &socketfd_set);
             
            //update the maximum file descriptor
            if(sd > max_socketfd)
                max_socketfd = sd;
        }
  
        //Wait for activity to show on one or more sockets
        int activity = select(max_socketfd + 1, &socketfd_set, NULL, NULL, NULL);
        //Should handle error but ignore for now
          
        //If something happened on the listening socket, then it's a new client
        if (FD_ISSET(listen_socketfd, &socketfd_set)) 
        {
            socklen_t addr_size = sizeof client_addr;
            incoming_socketfd = accept(listen_socketfd, (struct sockaddr *)&client_addr, &addr_size);
            if(incoming_socketfd == -1)
            {
                //Couldn't accept connection from client
                continue;
            }
                        
            //add new socket to array of sockets
            for (int i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if (client_socket[i] == 0)
                {
                    client_socket[i] = incoming_socketfd;
                    break;
                }
            }
        }
          
        //Otherwise it is an IO operation on a client connection
        for (int i = 0; i < max_clients; i++) 
        {
            //Get the socket that has activity
            int sd = client_socket[i];
            if (FD_ISSET(sd, &socketfd_set)) 
            {
                //Receive the message
                char* msg;
                if(recv_msg(sd, msg) <= 0)
                {
                    close(sd);
                    client_socket[i] = 0;
                    break;
                }

                cout << msg << endl;
                convert_to_titlecase(msg);
                
                //Prepare data for sending
                char* data;
                int length;
                length = serialize(string(msg), data);

                //Attempt to send the data
                if(send_all(sd, data, length) == -1)
                    handle_error("send_all");

                free(data);
                free(msg);
            }
        }
    }

    close(listen_socketfd);
    return 0;
}
