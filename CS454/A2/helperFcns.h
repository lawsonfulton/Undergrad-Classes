#ifndef helperFcns
#define helperFcns

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

#include <cstring>
#include <string>
#include <iostream>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int send_all(int sockfd, char *buf, int len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;

    while(total < len) {
        n = send(sockfd, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

//User is responsible for free()ing the returned data
//This fcn will allocate data and return its size
int serialize(std::string str, char*& data)
{
    int str_len = str.length() + 1;
    int size = str_len + 4;
    data = (char*)malloc(size);

    memcpy(data, &str_len, sizeof(str_len));
    strcpy(data + 4, str.c_str());

    return size;
}


//Blocks until exactly len bytes have been received
int recv_exact_bytes(int socketfd, char* buf, int len)
{
	int total = 0;
    while(total < len)
    {
        int bytes_received = 0;
        bytes_received = recv(socketfd, buf + total, len - total, 0);
        
        if (bytes_received <= 0)
            return bytes_received;

        total += bytes_received;
    }

    return total;
}

//Reads the 4 byte header of a message
//Returns the corresponding length of the remaining message
int recv_header(int socketfd, int* msg_len)
{
    return recv_exact_bytes(socketfd, (char*)msg_len, 4);
}

//Caller responsible for freeing msg_out
int recv_msg(int socketfd, char*& msg_out)
{
	int msg_len = 0;
	int status = recv_header(socketfd, &msg_len);

	if(status <= 0)
		return status;

	char* msg_raw = (char*)malloc(msg_len);
	status = recv_exact_bytes(socketfd, msg_raw, msg_len);
	msg_out = msg_raw;

	return status;
}

#endif






