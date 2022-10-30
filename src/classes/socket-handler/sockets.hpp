#pragma once

#include <exception>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/server.socket"
#define STD_SIZE 128

class SocketHandler {
    public:
        struct Message {
            std::string send_to;
            std::string sent_from;
            std::string message;

            Message() = default;

            Message(std::string send, std::string sent, std::string mess)
                : send_to { std::move(send) }
                , sent_from { std::move(sent) }
                , message { std::move(mess) }
                {};
        };

        static int connection_socket;

    public:
        static int start();

        static int openSocket(std::string name);

        static int listenClient(SocketHandler::Message* com, struct timeval dropout_time);

        static int listenServer(int* sockets, std::string* connection_list, int size, SocketHandler::Message* server_com, struct timeval dropout_time);

        static int sendMessage(int socket, Message com);

        static void closeSocket(int dis_socket);

    private:
        
        static int transfer(Message com, int* sockets, int size);

        static Message strToMsg(char* com);
};