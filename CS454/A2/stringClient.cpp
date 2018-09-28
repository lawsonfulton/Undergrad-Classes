#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include <cstring>
#include <iostream>
#include <queue>

#include "helperFcns.h"

using namespace std;

bool finished_reading = false;
queue<string> msg_queue;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

int msg_count = 0;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

void set_addr_and_port(char*& address, char*& port)
{
    address = getenv("SERVER_ADDRESS");
    port = getenv("SERVER_PORT");
    if (address == NULL)
    {
        cout << "Error: SERVER_ADDRESS not set" << endl;
        exit(1);
    }
    if (port == NULL) 
    {
        cout << "Error: SERVER_PORT not set" << endl;
        exit(1);
    }
}

void read_input_until_eof()
{
    string msg;
    while(getline(cin, msg))
    {
        pthread_mutex_unlock(&count_mutex);
        msg_count++;
        pthread_mutex_unlock(&count_mutex);

        pthread_mutex_lock(&queue_mutex);
        msg_queue.push(msg);
        pthread_mutex_unlock(&queue_mutex);
    }
    finished_reading = true;
}

void *sending_function(void *sockfd)
{
    while(!finished_reading || !msg_queue.empty())
    {
        //Check if the queue is empty
        pthread_mutex_lock(&queue_mutex);
        if(msg_queue.empty())
        {
            pthread_mutex_unlock(&queue_mutex);
            continue;
        }
        pthread_mutex_unlock(&queue_mutex);

        //Get the next message to send
        pthread_mutex_lock(&queue_mutex);
        string msg = msg_queue.front();
        msg_queue.pop();
        pthread_mutex_unlock(&queue_mutex);

        //Prepare data for sending
        char* data;
        int length;
        length = serialize(msg, data);

        //Attempt to send the data
        if(send_all(*(int*)sockfd, data, length) == -1)
            handle_error("send_all");

        free(data);

        sleep(2);
    }

    return 0;
}

void *receiving_function(void *sockfd)
{
    while(true)
    {
        //Receive until finished reading input and receiving all sent messages
        if(finished_reading && msg_count == 0) //Don't need mutex because only used after input finished
        {
            break;
        }

        //Read a response from the server
        char* msg;
        if(recv_msg(*(int*)sockfd, msg) <= 0)
        {
            cout << "Error: Server disconnected." << endl;
            exit(1);
        }

        cout << "Server: " << msg << endl;
        free(msg);

        pthread_mutex_unlock(&count_mutex);
        msg_count--;
        pthread_mutex_unlock(&count_mutex);
    }

    return 0;
}

int main () {
    int status;
    struct addrinfo hints, *servinfo;
    char *address, *port;
    int sockfd;

    //Set the destination port and address from env variables
    set_addr_and_port(address,port);

    //Set up socket information
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(address, port, &hints, &servinfo) != 0)
        handle_error("getaddrinfo");

    //Create the socket for connecting to the server
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(sockfd == -1)
        handle_error("socket");

    //Attempt to establish a connection with the server
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        handle_error("connect");
    
    //Start sending and receiving threads
    pthread_t send_thread, recv_thread;
    int ret_send_thread, ret_recv_thread;

    ret_send_thread = pthread_create(&send_thread, NULL, sending_function, (void*) &sockfd);
    ret_recv_thread = pthread_create(&recv_thread, NULL, receiving_function, (void*) &sockfd);

    //Get user input
    read_input_until_eof();

    //Wait until done sending and receiving
    pthread_join(send_thread, NULL);

    if(msg_count == 0)
        pthread_kill(recv_thread,0);
    else
        pthread_join(recv_thread, NULL);

    freeaddrinfo(servinfo);
    return 0;
}
