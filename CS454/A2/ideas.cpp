
int main()
{
//Get the socket that has activity
    int sd = client_socket[i];


    MessageListener listener;
    ServerBinderMessage message = listener.receive_message(); //Error detection? try/catch

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